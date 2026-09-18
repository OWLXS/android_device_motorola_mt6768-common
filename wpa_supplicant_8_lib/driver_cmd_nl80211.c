/*
 * Driver interaction with extended Linux CFG8021
 * MediaTek gen4m private command passthrough (SIOCDEVPRIVATE + 1)
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>

#include "common.h"
#include "linux_ioctl.h"
#include "driver_nl80211.h"
#ifdef ANDROID
#include "android_drv.h"
#endif

/*
 * Must match struct priv_driver_cmd_s in the gen4m driver
 * (os/linux/gl_wext_priv.c). The buffer is inline, not a pointer:
 * the driver is built without CFG_ANDROID_AOSP_PRIV_CMD.
 */
#define MTK_PRIV_CMD_SIZE 512

struct mtk_priv_driver_cmd {
	char buf[MTK_PRIV_CMD_SIZE];
	int used_len;
	int total_len;
};

static void wpa_driver_notify_country_change(void *ctx, char *cmd)
{
	if ((os_strncasecmp(cmd, "COUNTRY", 7) == 0) ||
	    (os_strncasecmp(cmd, "SETBAND", 7) == 0)) {
		union wpa_event_data event;

		os_memset(&event, 0, sizeof(event));
		event.channel_list_changed.initiator = REGDOM_SET_BY_USER;
		if (os_strncasecmp(cmd, "COUNTRY", 7) == 0) {
			event.channel_list_changed.type = REGDOM_TYPE_COUNTRY;
			if (os_strlen(cmd) > 9) {
				event.channel_list_changed.alpha2[0] = cmd[8];
				event.channel_list_changed.alpha2[1] = cmd[9];
			}
		} else {
			event.channel_list_changed.type = REGDOM_TYPE_UNKNOWN;
		}
		wpa_supplicant_event(ctx, EVENT_CHANNEL_LIST_CHANGED, &event);
	}
}

int wpa_driver_nl80211_driver_cmd(void *priv, char *cmd, char *buf,
				  size_t buf_len)
{
	struct i802_bss *bss = priv;
	struct wpa_driver_nl80211_data *drv = bss->drv;
	struct ifreq ifr;
	struct mtk_priv_driver_cmd priv_cmd;
	int ret = 0;

	if (os_strcasecmp(cmd, "STOP") == 0) {
		linux_set_iface_flags(drv->global->ioctl_sock, bss->ifname, 0);
		wpa_msg(drv->ctx, MSG_INFO, WPA_EVENT_DRIVER_STATE "STOPPED");
	} else if (os_strcasecmp(cmd, "START") == 0) {
		linux_set_iface_flags(drv->global->ioctl_sock, bss->ifname, 1);
		wpa_msg(drv->ctx, MSG_INFO, WPA_EVENT_DRIVER_STATE "STARTED");
	} else if (os_strcasecmp(cmd, "MACADDR") == 0) {
		u8 macaddr[ETH_ALEN] = {};

		ret = linux_get_ifhwaddr(drv->global->ioctl_sock, bss->ifname,
					 macaddr);
		if (!ret)
			ret = os_snprintf(buf, buf_len,
					  "Macaddr = " MACSTR "\n",
					  MAC2STR(macaddr));
	} else {
		if (os_strlen(cmd) >= MTK_PRIV_CMD_SIZE) {
			wpa_printf(MSG_ERROR, "%s: command too long: %s",
				   __func__, cmd);
			return -1;
		}

		os_memset(&ifr, 0, sizeof(ifr));
		os_memset(&priv_cmd, 0, sizeof(priv_cmd));
		os_strlcpy(ifr.ifr_name, bss->ifname, IFNAMSIZ);
		os_strlcpy(priv_cmd.buf, cmd, MTK_PRIV_CMD_SIZE);
		priv_cmd.used_len = os_strlen(cmd) + 1;
		priv_cmd.total_len = MTK_PRIV_CMD_SIZE;
		ifr.ifr_data = &priv_cmd;

		ret = ioctl(drv->global->ioctl_sock, SIOCDEVPRIVATE + 1, &ifr);
		if (ret < 0) {
			wpa_printf(MSG_ERROR,
				   "%s: private command \"%s\" failed: %s",
				   __func__, cmd, strerror(errno));
			return ret;
		}

		ret = 0;
		wpa_driver_notify_country_change(drv->ctx, cmd);

		/* Only GET-style commands report a reply length back */
		if ((os_strcasecmp(cmd, "LINKSPEED") == 0) ||
		    (os_strcasecmp(cmd, "RSSI") == 0) ||
		    (os_strcasecmp(cmd, "GETBAND") == 0)) {
			size_t n = priv_cmd.used_len > 0 ?
				(size_t) priv_cmd.used_len : 0;

			if (n > MTK_PRIV_CMD_SIZE)
				n = MTK_PRIV_CMD_SIZE;
			if (n >= buf_len)
				n = buf_len - 1;
			os_memcpy(buf, priv_cmd.buf, n);
			buf[n] = '\0';
			ret = os_strlen(buf);
		}
	}
	return ret;
}

int wpa_driver_set_p2p_noa(void *priv, u8 count, int start, int duration)
{
	return 0;
}

int wpa_driver_get_p2p_noa(void *priv, u8 *buf, size_t len)
{
	return 0;
}

int wpa_driver_set_p2p_ps(void *priv, int legacy_ps, int opp_ps, int ctwindow)
{
	return -1;
}

int wpa_driver_set_ap_wps_p2p_ie(void *priv, const struct wpabuf *beacon,
				 const struct wpabuf *proberesp,
				 const struct wpabuf *assocresp)
{
	return 0;
}
