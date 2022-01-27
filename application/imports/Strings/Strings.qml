pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property string errorStyle:
"<style type='text/css'>p { margin-top: 0px; margin-bottom: 5px; } a { color: #fe8a2c; } </style>"

    readonly property string errorInvalidDevice:
"<p>Device cannot be recognized.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Disconnect all other devices from this computer.</p>
<p>2. Reconnect device via USB.</p>
<p>3. Grant user permissions to access DFU devices.</p>
<p>Run <a href='https://github.com/flipperdevices/qFlipper/raw/dev/setup_rules.sh'>this script</a> on Linux or check drivers on Windows.</p>
<p>4. Reboot device and try again.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorSerial:
"<p>Flipper is not responding.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Check that device is not connected via Bluetooth or Terminal session.</p>
<p>2. Grant user permissions to access Serial devices.</p>
<p>Run <a href='https://github.com/flipperdevices/qFlipper/raw/dev/setup_rules.sh'>this script</a> on Linux or check drivers on Windows.</p>
<p>3. Reconnect device via USB.</p>
<p>4. Reboot device and try again.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorRecovery:
"<p>Cannot connect to Flipper in Update & Recovery mode. Device not found.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Check that device in Update & Recovery mode.</p>
<p>2. Reconnect device via USB.</p>
<p>3. Grant user permissions to access DFU devices.</p>
<p>For Windows, check that device drivers have been correctly installed.</p>
<p>For Linux, run <a href='https://github.com/flipperdevices/qFlipper/raw/dev/setup_rules.sh'>this script</a>.</p>
<p>4. Reboot device and try again.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorInternet:
"<p>Cannot connect to update server.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Check your internet connection.</p>
<p>2. Ensure that the update server is not down.</p>
<p>3. Try updating again.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorProtocol:
"<p>The device firmware seems to be too old.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Reboot device into Recovery mode.</p>
<p>2. Click Repair to perform a clean installation.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorDisk:
"<p>Can’t save or read files to/from the local filesystem</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Make sure that you have free space on your local drive.</p>
<p>2. Check that qFlipper has permissions to write on disk.</p>
<p>3. When applicable, make sure to point qFlipper to the right files/directores.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorTimeout:
"<p>The operation is taking too long.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Check USB cable.</p>
<p>2. Reboot the device.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorUnknown:
"<p>Something utterly strange has happened.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Turn it off and on again.</p>
<p>2. Do a barrel roll!.</p>
<p>3. ...</p>
<p>4. PROFIT!</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"
}
