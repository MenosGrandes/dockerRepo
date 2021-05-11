#!/bin/bash
cd /root/code
git clone https://gitlab.com/MenosGrandes/pupillabupgrades.git pupil_labs
cd pupil_labs
git checkout task/mg/updateForkPupilLab #Current development branch
python3 -m pip install -r requirements.txt

