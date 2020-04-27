#include <stdio.h>
#include "StreamPlayer.h"

int main(int argc, char** argv)
{
	av_log_set_level(AV_LOG_ERROR);
	if (argc < 2)
	{
		printf("Usage: %s [file_name]\n", argv[0]);
		return 0;
	}

	StreamPlayer player(argv[1]);
	player.start();

	return 0;
}