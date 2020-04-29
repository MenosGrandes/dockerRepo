#!/bin/bash
docker run -d -p 6080:6080 dosbox
xdg-open  http://localhost:6080/vnc.html
