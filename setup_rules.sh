#!/bin/sh

RULES_DIR=/etc/udev/rules.d
RULES_FILE="$RULES_DIR"/42-flipperzero.rules
RULES_TEXT='#Flipper Zero serial port
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", ATTRS{manufacturer}=="Flipper Devices Inc.", TAG+="uaccess"
#Flipper Zero DFU
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", ATTRS{manufacturer}=="STMicroelectronics", TAG+="uaccess"'

warning_message() {
    printf "You will now be asked for SUDO password.\n"
}

rules_install() {
    warning_message

    sudo -K &&
    # The danger zone
    printf "%s\n" "$RULES_TEXT" | sudo tee "$RULES_FILE" > /dev/null &&
    sudo udevadm control --reload-rules &&
    sudo udevadm trigger
    # End of danger zone

    if [ $? -eq 0 ]; then
        printf "Device rules have been installed successfully. You're good to go!\n"
    else
        printf "Something went wrong. Device rules may have not been installed correctly.\n"
    fi
}

rules_uninstall() {
    if [ -f "$RULES_FILE" ]; then
        warning_message

        sudo -K &&
        # The danger zone
        sudo rm -rf $RULES_FILE &&
        sudo udevadm control --reload-rules &&
        sudo udevadm trigger
        # End of danger zone

        if [ $? -eq 0 ]; then
            printf "Device rules have been uninstalled successfully. Thank you.\n"
        else
            printf "Something went wrong. Device rules may have not been uninstalled correctly.\n"
        fi

    else
        printf "Nothing to uninstall. Bye!\n"
    fi
}

clear

if ! [ -d "$RULES_DIR" ]; then
    printf "Your system seems to have an unusual Udev rules directory, please check your distro's documentation and edit the RULES_DIR variable accordingly."
    exit
fi

printf "This script will install system rules that will enable communication with your Flipper Zero.\n"

while true
do
    printf "Choose what to do: [I]nstall, [U]ninstall or [E]xit: "
    read REPLY
    case $REPLY in
        [Ii]* ) rules_install; break;;
        [Uu]* ) rules_uninstall; break;;
        [Ee]* ) printf "Bye!\n"; exit;;
        * ) printf "Please enter one of the letters: I, U or E.\n";;
    esac
done
