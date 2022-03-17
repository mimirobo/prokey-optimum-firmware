#include <string.h>



#include "bootloader.h"
#include "buttons.h"
#include "ecdsa.h"
#include "layout.h"
#include "memory.h"
#include "memzero.h"
#include "oled.h"
#include "rng.h"
#include "secp256k1.h"
#include "sha2.h"
#include "aes/aes.h"
#include "signatures.h"
#include "usb.h"
#include "util.h"
#include "auth.h"
#include "udp.h"

#if !EMULATOR
#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>
#include "usb21_standard.h"
#include "usb_desc.h"
#else

#endif
#include "usb_erase.h"
#include "usb_send.h"

enum {
  STATE_READY,
  STATE_OPEN,
  STATE_FLASHSTART,
  STATE_FLASHING,
  STATE_END,
};

static uint32_t flash_pos = 0, flash_len = 0;
static char flash_state = STATE_READY;
static uint8_t flash_anim = 0;
static uint32_t stackPointer = 0;
aes_decrypt_ctx decCtx;

void    CmdOnReceive    (usbd_device *dev, unsigned char* buf)
{
    static uint16_t msg_id = 0xFFFF;
    static uint8_t toDecript[16] __attribute__((aligned(4)));
    static int di;

    if (flash_state == STATE_END) {

    msg_id = (buf[3] << 8) + buf[4];
    
    if (msg_id == 0xFFF0) 
		{		
			DeviceReset(true);
			return;
		}
		return;
  }

  if (flash_state == STATE_READY || flash_state == STATE_OPEN ||
      flash_state == STATE_FLASHSTART ) {
    if (buf[0] != '?' || buf[1] != '#' ||
        buf[2] != '#') {  // invalid start - discard
      return;
    }
    // struct.unpack(">HL") => msg, size
    msg_id = (buf[3] << 8) + buf[4];
  }

  if (flash_state == STATE_READY || flash_state == STATE_OPEN) {

    //! Prokey Commands
		//! Prokey Restart (id 65520)
		if (msg_id == USB_MSG_ID_RESTART_REG) 
		{		
			DeviceReset(false);
			return;
		}

		//! Prokey Challenge (id 65521)
		if (msg_id == USB_MSG_ID_CHALLENGE_REQ) 
		{ 	
			flash_state = STATE_OPEN;
			sAuthResponse ar;
			if( AuthNext( buf, 9, &ar ) == false )
			{
				sendMsgFailureWithReason(dev, ar.response[0]);
				return;
			}

      if( AuthIsOkay() )
      {
        layoutDialog(&bmp_icon_info, NULL, NULL, NULL, "Your Prokey is", "genuine", NULL, NULL, NULL, NULL);
      }

			SendPacketToUsb( dev, USB_MSG_ID_CHALLENGE_RES, ar.response, ar.len );
			return;
		}

    //! Get Authenticate status
    if (msg_id == USB_MSG_ID_AUTH_STAT_REQ)
    {
      sAuthResponse ar;
      AuthStatus(&ar);
      SendPacketToUsb( dev, USB_MSG_ID_AUTH_STAT_RES, ar.response, ar.len);
      return;
    }

    //! Request to set the AuthKey in OTP
    if(msg_id == USB_MSG_ID_SET_OTP_REQ)
    {
      sAuthResponse ar;
      //! If OTP is already set, the function returns error
      if( AuthSetKey(&ar) == false )
      {
        sendMsgFailureWithReason(dev, ar.response[0]);
      }
      else
      {
        SendPacketToUsb( dev, USB_MSG_ID_SET_OTP_RES, ar.response, ar.len);
      }

      return;
    }

    //! Request ACK
    if(msg_id == USB_MSG_ID_OTP_WRITE_REQ)
    {
      sAuthResponse ar;
      if(AuthWriteAuthKeyToOpt(buf, 9, &ar) == false)
      {
        sendMsgFailureWithReason(dev, ar.response[0]);
      }
      else
      {
        SendPacketToUsb(dev, USB_MSG_ID_OTP_WRITE_RES, ar.response, ar.len);
      }
      return;
    }

    if (msg_id == USB_MSG_ID_INITIALIZE) 
    {  // Initialize message (id 0)
      send_msg_features(dev);
      flash_state = STATE_OPEN;
      return;
    }

    if (msg_id == USB_MSG_ID_GET_FEATURES) 
    {  // GetFeatures message (id 55)
      send_msg_features(dev);
      return;
    }

    if (msg_id == USB_MSG_ID_PING) 
    {  // Ping message (id 1)
      send_msg_success(dev);
      return;
    }
  }

  if( AuthIsOkay() == false )
  {
    sendMsgFailureWithReason(dev, AUTH_ERR_UNAUTHORIZED);
    return;
  }

  if (flash_state == STATE_READY || flash_state == STATE_OPEN)
  {
    if (msg_id == USB_MSG_ID_WIPE) 
    {  // WipeDevice message (id 5)
      layoutDialog(&bmp_icon_question, "Cancel", "Confirm", NULL, "Do you really want to", "wipe the device?", NULL, "All data will be lost.", NULL, NULL);
      bool but = get_button_response();

      if( but )
      {
        layoutDialog(&bmp_icon_question, "Cancel", "Confirm", NULL, "Never do this without", "your recovery sheet.", NULL, "All data will be lost.", NULL, NULL);
        but = get_button_response();
        if (but) 
        {
          erase_storage_code_progress();

          flash_state = STATE_END;
          layoutDialog(&bmp_icon_ok, NULL, NULL, NULL, "Device", "wiped.", NULL, "You may now", "unplug your Prokey.", NULL);
          send_msg_success(dev);

          return;
        } 
      }

      flash_state = STATE_END;
      layoutDialog(&bmp_icon_warning, NULL, NULL, NULL, "Device wipe", "aborted.", NULL, "You may now", "unplug your Prokey.", NULL);
      send_msg_failure(dev);
      return;
    }
  }

  if (flash_state == STATE_OPEN) 
  {
    if (msg_id == USB_MSG_ID_ERASE_FIRMWARE) 
    {  // FirmwareErase message (id 6)

      bool proceed = false;
      if( firmware_present() )
      {
        layoutDialog(&bmp_icon_question, "Abort", "Continue", NULL, "Install new", "firmware?", NULL, "Never do this without", "your recovery card!", NULL);
        proceed = get_button_response();
      }
      else 
      {
        proceed = true;
      }

      if( proceed )
      {
          erase_code_progress();

          send_msg_success(dev);
          flash_state = STATE_FLASHSTART;

          return;
      }

      send_msg_failure(dev);
			flash_state = STATE_END;
			layoutDialog(&bmp_icon_warning, NULL, NULL, NULL, "Firmware installation", "canceled.", NULL, "You may now", "unplug your Prokey.", NULL);
			return;
    }

    return;
  }

  if (flash_state == STATE_FLASHSTART) 
  {
    if(msg_id == USB_MSG_ID_WRITE_FIRMWARE)
		{
			//! Firmware Lenght
			//! Length-delimited type
			if(buf[9] != 0x0A)
			{
				send_msg_failure(dev);
				flash_state = STATE_END;
				layoutDialog(&bmp_icon_error, NULL, NULL, NULL, "Error installing ", "firmware.", NULL, "Unplug your Prokey", "and try again.", NULL);
				return;
			}

			const uint8_t *p = &buf[10];
			flash_len = readprotobufint(&p);

			if (flash_len > FLASH_TOTAL_SIZE - (FLASH_APP_START - FLASH_ORIGIN)) // firmware is too big
			{ 
				send_msg_failure(dev);
				flash_state = STATE_END;
				layoutDialog(&bmp_icon_error, NULL, NULL, NULL, "Firmware is too big.", NULL, NULL, NULL, NULL, NULL);
				return;
			}

			aes_init();
			aes_decrypt_key256(AuthGet()->sessionKeyHash, &decCtx );

			flash_state = STATE_FLASHING;
			flash_pos=0;
			di = 0;
			bool isFirst = true;

			flash_unlock();
			while (p < buf + 64) 
			{
				toDecript[di++] = *p;
				if( di == 16 )
				{
					di = 0;

					uint8_t plain[16] __attribute__((aligned(4)));
					aes_decrypt( toDecript, plain, &decCtx );
					
					for( int i=0; i<16; i+=4 )
					{
						if( isFirst )
						{
							isFirst = false;
							stackPointer = plain[i];
							stackPointer |= plain[i+1] << 8;
							stackPointer |= plain[i+2] << 16;
							stackPointer |= plain[i+3] << 24;
							flash_pos += 4;
							continue;
						}

						const uint32_t *w = (uint32_t *)&plain[i];
						flash_program_word(FLASH_APP_START + flash_pos, *w);
						flash_pos += 4;
					}
					
				}
				p++;
			}
			flash_lock();

			return;
		}
		return;
  }

  if (flash_state == STATE_FLASHING) 
  {
    if (buf[0] != '?') // invalid contents
		{	
			send_msg_failure(dev);
			flash_state = STATE_END;
			layoutDialog(&bmp_icon_error, NULL, NULL, NULL, "Error installing ", "firmware.", NULL, "Unplug your ProKey", "and try again.", NULL);
			return;
		}

		const uint8_t *p = buf + 1;
		if (flash_anim % 400 == 4) {
			layoutProgress("Installing, Please wait", 1000 * flash_pos / flash_len);
		}
		flash_anim++;
		flash_unlock();
		while (p < buf + 64 && flash_pos < flash_len) 
		{
			toDecript[di++] = *p;
			if( di == 16 )
			{
				di = 0;
				
				uint8_t plain[16] __attribute__((aligned(4)));
				aes_decrypt( toDecript, plain, &decCtx );
				
				for( int i=0; i<16; i+=4 )
				{
					const uint32_t *w = (uint32_t *)&plain[i];
					flash_program_word(FLASH_APP_START + flash_pos, *w);
					flash_pos += 4;
				}
				
			}
			p++;
		}
		flash_lock();

		// flashing done
		if (flash_pos == flash_len) 
		{
      //! The reason we check the SP here is that flashing the firmware is a time consuming process and this time prevents attacker(man in the middle) to
      //! brute force different Encrypted Key
      //TODO: Better to check a magic to make sure Encrypted Key is correct.
      if((stackPointer & 0x2FFE0000) != 0x20000000)
      {
        send_msg_failure(dev);
        flash_state = STATE_END;
        layoutDialog(&bmp_icon_error, NULL, NULL, NULL, "Error installing ", "firmware.", NULL, "Unplug your ProKey", "and try again.", "ERR:SP");
        return;
      }

			flash_unlock();
			flash_program_word(FLASH_APP_START, stackPointer);
			flash_lock();

			layoutDialog(&bmp_icon_ok, NULL, NULL, NULL, "New firmware", "successfully installed.", NULL, "You may now", "unplug your Prokey.", NULL);
			send_msg_success(dev);

			flash_state = STATE_END;
			return;
		} 
		else 
		{
			return;
		}
  }

  //! MsgId not supported
  sendMsgFailureWithReason(dev, 0xFF);
}