#!/bin/sh
set -e

RULES_DIR=/etc/udev/rules.d
RULES_FILE="$RULES_DIR"/42-flipperzero.rules
RULES_TEXT='#Flipper Zero serial port
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", ATTRS{manufacturer}=="Flipper Devices Inc.", TAG+="uaccess"
#Flipper Zero DFU
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", ATTRS{manufacturer}=="STMicroelectronics", TAG+="uaccess"
#Flipper ESP32s2 BlackMagic
SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="40??", ATTRS{manufacturer}=="Flipper Devices Inc.", TAG+="uaccess"
'

# Detect a privilege escalation tool. Sets the following globals:
#   PRIVSEP        — command to prefix elevated operations with ("", "sudo", or "doas")
#   PRIVSEP_RESET  — command to drop any cached credential ("sudo -K" for sudo, ":" otherwise)
#
# Order of preference: already-root, sudo, doas. Returns non-zero if none apply.
detect_privsep() {
    if [ "$(id -u)" -eq 0 ]; then
        PRIVSEP=""
        PRIVSEP_RESET=":"
        return 0
    fi
    if command -v sudo >/dev/null 2>&1; then
        PRIVSEP="sudo"
        PRIVSEP_RESET="sudo -K"
        return 0
    fi
    if command -v doas >/dev/null 2>&1; then
        PRIVSEP="doas"
        PRIVSEP_RESET=":"
        return 0
    fi
    printf "Error: installing udev rules requires write access to %s,\nbut neither 'sudo' nor 'doas' is available and you are not root.\nRe-run this script as root, or install one of the above.\n" "$RULES_DIR" >&2
    return 1
}

warning_message() {
    if [ -n "$PRIVSEP" ]; then
        printf "You may be asked for your %s password.\n" "$PRIVSEP"
    fi
}

rules_install() {
    detect_privsep || exit 1
    warning_message

    # The danger zone
    if \
            $PRIVSEP_RESET \
            && printf "%s\n" "$RULES_TEXT" | $PRIVSEP dd of="$RULES_FILE" >/dev/null 2>&1 \
            && $PRIVSEP udevadm control --reload-rules \
            && $PRIVSEP udevadm trigger
    # End of danger zone
    then
        printf "Device rules have been installed successfully. You're good to go!\n"
    else
        printf "Something went wrong. Device rules may have not been installed correctly.\n"
    fi
}

rules_uninstall() {
    if [ -f "$RULES_FILE" ]; then
        detect_privsep || exit 1
        warning_message

        # The danger zone
        if
                $PRIVSEP_RESET \
                && $PRIVSEP rm -rf "$RULES_FILE" \
                && $PRIVSEP udevadm control --reload-rules \
                && $PRIVSEP udevadm trigger
        # End of danger zone
        then
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
    read -r REPLY
    case $REPLY in
        [Ii]* ) rules_install; break;;
        [Uu]* ) rules_uninstall; break;;
        [Ee]* ) printf "Bye!\n"; exit;;
        * ) printf "Please enter one of the letters: I, U or E.\n";;
    esac
done
