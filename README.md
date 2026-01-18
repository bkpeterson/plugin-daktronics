# Daktronics Scoreboard Plugin

## Introduction

This OBS Studio plugin reads data in real-time from a Daktronics scoreboard controller such as the AllSport 5000.  The connection is via RS-232 serial interface, so you will need a DB-25 male connector to USB cable to connect the scoreboard controller to a computer.  My setup involves a DB-25 to DB-9 adapter, then a DB-9 to USB adapter cable as they are easier to find.

The plugin creates a custom dock which can be activated via the "Tools" menu in OBS.  This dock allows you to select the appropriate serial port, view the serial connection status, and activate a full-screen preview to output the scoreboard.

To use the filter, create a source to display a scoreboard data field, add the Daktronics scoreboard filter, and select the filter type from the filter properties:
* Show/Hide - This will toggle the visibility of the source depending on whether the selected scoreboard data field is blank or not (:00 is considered blank)
* Update Text - This will update the selected parameter of the source with the data from the selected scoreboard field as it is received from the controller
* Update Color - This will change the color of the selected parameter for the source based on whether or not the selected scoreboard field is blank (or :00)

* Parameter - for text and color updates, select the parameter name of the source which should be updated based on the scoreboard data

* Invert logic - This applies to the Show/Hide setting and will reverse the toggle to show a source when blank (or :00) instead of hide it when blank.  An example might be a red border around the game clock that is displayed when the clock reaches zero.  Alternatively, a visible horn indicator may be displayed when the scoreboard data field for the horn is NOT blank.

The "Sport" and "Field" parameters select the sport for the current scoreboard mode, which updates the list of available scoreboard data fields to select from.

An example that utilizes all 3 filters would be a timeout clock.  You would add a text source to display the timeout clock and add a Daktronics Scoreboard Filter with the "Update Text" option, and select the text parameter for the source.  From the field drop-down, select the appropriate scoreboard data field that contains the timeout clock display.  You could also add another Daktronics Scoreboard Filter to the same text source of type "Show/Hide", and again select the timeout clock field.  This will make the clock visible only when the timeout clock is not zero.  You could also add a border around the clock that is one color, but use the "Update Color" filter option to change the border to a different color when the timeout clock field is not zero.

An important thing to note is that the Daktronics scoreboard controllers only send data for fields that have changed.  When the main clock is stopped, it will send a data string to update all fields.  When you first start OBS Studio, you might need to start/stop or stop/start the scoreboard clock to allow all of the data fields to synchronize.

This has been tested on MacOS, so please submit feedback for other platforms.
