#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_ID 0x123

int main(int argc, char **argv)
{
	int s;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
	const char *ifname = "can0";
	char buf[16];

	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
		perror("socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	fprintf(stderr, "index=%d\n", addr.can_ifindex);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(s);
		return -1;
	}

	frame.can_id = CAN_ID;
	frame.can_dlc = 7;
	memset(&frame.data[0], 0, 8);
	memcpy(&frame.data[0], "request", 7);

	nbytes = write(s, &frame, sizeof(struct can_frame));
	if (nbytes < 0) {
		perror("write");
		goto exit;
	}

	fprintf(stderr, "%d bytes are sent\n\n", nbytes);

	nbytes = -1;
	memset(&frame, 0, sizeof(struct can_frame));
	nbytes = read(s, &frame, sizeof(struct can_frame));
	if (nbytes < 0) {
		perror("read");
		goto exit;
	}

	memset(buf, 0, sizeof(buf));
	memcpy(buf, frame.data, frame.can_dlc);

	fprintf(stderr, "%d bytes are received\n", nbytes);
	fprintf(stderr, "Received message: %s (%d bytes)\n", buf, frame.can_dlc);

exit:
	close(s);
	return -1;
}

