#!/bin/bash
clear
killall memcheck-amd64-
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes  --track-origins=yes ./VisualBotNGine -doNotMove 2> Debug.log&
exit 0
