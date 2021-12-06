#!/usr/bin/env bash

profiles=(
	"https://sample1"
	"https://sample2"
)

# backup preprocess
if [[ $(id -u) != "0" ]]; then
	echo "You cannot run this script without root."	
	exit 1
fi

CONFIG="config.yaml"
if [[ -e "${CONFIG}.bak" ]]; then
	rm "${CONFIG}.bak"
fi
if [[ -e ${CONFIG} ]]; then
	mv config.yaml config.yaml.bak
fi

profiles_len=$(expr ${#profiles[@]} - 1)
print_profiles() {
	echo "profile list"
	for i in $(seq 0 $profiles_len); do
		echo "$i) ${profiles[i]}"
	done	
}

update_profiles() {
	wget --output-document "${CONFIG}" -q $1 2> /dev/null
	sed -i "11 i ipv6: true" "${CONFIG}"
	systemctl restart clash
}

print_profiles
output="please enter the index of profiles"
while true; do
	echo "$output i.e. [0.. ${profiles_len}]:"
	read profile_num
	if [[ $profile_num -le $profiles_len && $profile_num -ge 0 ]]; then 
		break
	fi
	output="please re-enter the index of profiles"
done

update_profiles ${profiles[$profile_num]}
