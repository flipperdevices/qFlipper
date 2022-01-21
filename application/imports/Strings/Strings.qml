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
<p>Run <a href='https://github.com/flipperdevices/qFlipper/raw/dev/setup_rules.sh'>this script</a> for Linux.</p>
<p>4. Reboot device to Flipper OS and try updating again.</p>
<p>-----------------------------------</p>
<center><a href='https://docs.flipperzero.one/'>READ MORE</a></center>"

    readonly property string errorRecovery:
"<p>Cannot connect to Flipper in Update & Recovery mode. Device not found.</p>
<p>=========== HOW TO FIX ============</p>
<p>1. Check that device in Update & Recovery mode.</p>
<p>2. Reconnect device via USB.</p>
<p>3. Grant user permissions to access DFU devices.</p>
<p>Run <a href='https://github.com/flipperdevices/qFlipper/raw/dev/setup_rules.sh'>this script</a> for Linux.</p>
<p>4. Reboot device to Flipper OS and try updating again.</p>
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
