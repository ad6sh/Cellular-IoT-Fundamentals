/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <modem/lte_lc.h>
#include <zephyr/random/rand32.h>
#include <dk_buttons_and_leds.h>

/* STEP 2.2 - Include the header file for the CoAP library */


/* STEP 4.1 - Define the macro for the message from the board */


/* STEP 4.2 - Define the macros for the CoAP version and message length */


/* STEP 5 - Declare the buffer coap_buf to receive the response. */


/* STEP 6.1 - Define the CoAP message token next_token */


static int sock;
static struct sockaddr_storage server;

K_SEM_DEFINE(lte_connected, 0, 1);


LOG_MODULE_REGISTER(Lesson5_Exercise1, LOG_LEVEL_INF);

/**@brief Resolves the configured hostname. */
static int server_resolve(void)
{
	int err;
	struct addrinfo *result;
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM
	};
	char ipv4_addr[NET_IPV4_ADDR_LEN];

	err = getaddrinfo(CONFIG_COAP_SERVER_HOSTNAME, NULL, &hints, &result);
	if (err != 0) {
		LOG_ERR("ERROR: getaddrinfo failed %d\n", err);
		return -EIO;
	}

	if (result == NULL) {
		LOG_ERR("ERROR: Address not found\n");
		return -ENOENT;
	}

	/* IPv4 Address. */
	struct sockaddr_in *server4 = ((struct sockaddr_in *)&server);

	server4->sin_addr.s_addr =
		((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
	server4->sin_family = AF_INET;
	server4->sin_port = htons(CONFIG_COAP_SERVER_PORT);

	inet_ntop(AF_INET, &server4->sin_addr.s_addr, ipv4_addr,
		  sizeof(ipv4_addr));
	LOG_INF("IPv4 Address found %s\n", ipv4_addr);

	/* Free the address. */
	freeaddrinfo(result);

	return 0;
}

static void lte_handler(const struct lte_lc_evt *const evt)
{
     switch (evt->type) {
     case LTE_LC_EVT_NW_REG_STATUS:
             if ((evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_HOME) &&
             (evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_ROAMING)) {
                     break;
             }

             LOG_INF("Connected to: %s network",
             evt->nw_reg_status == LTE_LC_NW_REG_REGISTERED_HOME ? "home" : "roaming");
			 k_sem_give(&lte_connected);
             break;
     default:
             break;
     }
}

static void modem_configure(void)
{
	int err = lte_lc_init_and_connect_async(lte_handler);
	if (err) {
		LOG_INF("Modem could not be configured, error: %d", err);
		return;
	}
}

/**@brief Initialize the CoAP client */
static int client_init(void)
{
	int err;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create CoAP socket: %d.\n", errno);
		return -errno;
	}

	err = connect(sock, (struct sockaddr *)&server,
		      sizeof(struct sockaddr_in));
	if (err < 0) {
		LOG_ERR("Connect failed : %d\n", errno);
		return -errno;
	}

	/* STEP 6.2 - Generate a random token after the socket is connected */


	return 0;
}

/**@brief Handles responses from the remote CoAP server. */
static int client_handle_get_response(uint8_t *buf, int received)
{
	/* STEP 9.1 - Parse the received CoAP packet */

	/* STEP 9.2 - Confirm the token in the response matches the token sent */

	/* STEP 9.3 - Retrieve the payload and confirm it's nonzero */

	/* STEP 9.4 - Log the header code, token and payload of the response */

	return 0;
}

/**@biref Send CoAP GET request. */
static int client_get_send(void)
{
	int err;

	/* STEP 7.1 - Create the CoAP message*/

	/* STEP 7.2 - Add an option specifying the resource path */

	/* STEP 7.3 - Send the configured CoAP packet */

	return 0;
}

/**@biref Send CoAP PUT request. */
static int client_put_send(void)
{
	int err;
	struct coap_packet request;

	next_token++;

	/* STEP 8.1 - Initialize the CoAP packet and append the resource path */ 

	/* STEP 8.2 - Append the content format as plain text */

	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	/* STEP 8.3 - Add the payload to the message */

	err = send(sock, request.data, request.offset, 0);
	if (err < 0) {
		LOG_ERR("Failed to send CoAP request, %d\n", errno);
		return -errno;
	}

	LOG_INF("CoAP request sent: token 0x%04x\n", next_token);

	return 0;
}

static void button_handler(uint32_t button_state, uint32_t has_changed) 
{
	/* STEP 10 - Send a GET request or PUT request upon button triggers */

}

void main(void)
{
	int err, received;
	modem_configure();
	LOG_INF("Connecting to LTE network, this may take several minutes...");
	k_sem_take(&lte_connected, K_FOREVER);	
	err = dk_buttons_init(button_handler);
	if (err){
		LOG_ERR("Failed to initlize the Buttons Library");
	}
	err = dk_leds_init();
	if (err){
		LOG_ERR("Failed to initlize the LEDs Library");
	}
	dk_set_led_on(DK_LED2);	

	if (server_resolve() != 0) {
		LOG_ERR("Failed to resolve server name\n");
		return;
	}

	if (client_init() != 0) {
		LOG_ERR("Failed to initialize CoAP client\n");
		return;
	}

	while (1) {
		/* STEP 11 - Receive response from the CoAP server */

		/* STEP 12 - Parse the receive d CoAP packet */
	}

	(void)close(sock);
}
