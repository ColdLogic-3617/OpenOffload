/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.gappleto.common;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.DriverStationLCD;
import edu.wpi.first.wpilibj.DriverStationLCD.Line;

/**
 * This is a custom logging tool, which will include timestamp and voltage.
 * @author Gabe
 * @since Bruce (2.2.2)
 */
public class Log	{
	
    private static final DriverStation Station = DriverStation.getInstance();
    private static final DriverStationLCD LCD = DriverStationLCD.getInstance();
    
    private static int currentLine = 1;
    
    private static final Line    line1 = DriverStationLCD.Line.kUser1,
				 line2 = DriverStationLCD.Line.kUser2,
				 line3 = DriverStationLCD.Line.kUser3,
				 line4 = DriverStationLCD.Line.kUser4,
				 line5 = DriverStationLCD.Line.kUser5,
				 line6 = DriverStationLCD.Line.kUser6;
    
    /**
     * This custom logging tool will add the timestamp* and battery voltage to
     * any logging message for the robot.
     * *Note: on teleop, the time will start at 15 seconds.  This is not
     * necessarily the actual time.
     * @author Gabe
     * @param message	String—the message to be logged
     * @see PrintStream
     * @see DriverStation
     * @since Bruce (2.2.2)
     * @deprecated
     */
    public void printlnDep(String message)	{
	double v = Station.getBatteryVoltage();
	double t = Station.getMatchTime();
	v = (double)(((int)(v * 10000))/10000.0);
	t = (double)(((int)(t * 10000))/10000.0);
	System.out.println("[" + t + "] v=" + v + ": " + message);
    }
    
    /**
     * This custom logging tool will add the timestamp* and battery voltage to
     * any logging message for the robot.
     * *Note: on teleop, the time will start at 15 seconds.  This is not
     * necessarily the actual time.
     * @author Gabe
     * @param message	String—the message to be logged
     * @see PrintStream
     * @see DriverStation
     * @since Bruce (2.5.0)
     */
    public static void println(String message)	{
	double v = Station.getBatteryVoltage();
	double t = Station.getMatchTime();
	v = (double)(((int)(v * 10000))/10000.0);
	t = (double)(((int)(t * 10000))/10000.0);
	System.out.println("[" + t + "] v=" + v + ": " + message);
    }
    
    
    
    /**
     * This custom logging tool will add the timestamp* and battery voltage to
     * any logging message for the robot. It should also mirror this log entry
     * on the "LCD" portion of the Drive Station.
     * *Note: on teleop, the time will start at 15 seconds.  This is not
     * necessarily the actual time.
     * @author Gabe
     * @param message	String—the message to be logged
     * @see PrintStream
     * @see DriverStation
     * @see DriverStationLCD
     * @since Bruce (2.6.0)
     */
    public static void printlnConsBeta(String message)	{
	double v = Station.getBatteryVoltage();
	double t = Station.getMatchTime();
	v = (double)(((int)(v * 10000))/10000.0);
	t = (double)(((int)(t * 10000))/10000.0);
	message = "[" + t + "] v=" + v + ": " + message;
	System.out.println(message);
	putToLCD(message);
    }
    
    /**
     * Internal method to put data onto the LCD. This should not be publicly
     * accessible, as it is only meant to be accessed from the traditional
     * log method. This may change in the future.
     * @author Gabe
     * @param message	String-the message to be logged
     * @see DriverStationLCD
     * @since Bruce (2.6.0)
     */
    private static void putToLCD(String message)	{
    	if (currentLine % 6 == 1)
	    LCD.println(line1, 1, message);
	else if (currentLine % 6 == 2)
	    LCD.println(line2, 1, message);
	else if (currentLine % 6 == 3)
	    LCD.println(line3, 1, message);
	else if (currentLine % 6 == 4)
	    LCD.println(line4, 1, message);
	else if (currentLine % 6 == 5)
	    LCD.println(line5, 1, message);
	else
	    LCD.println(line6, 1, message);
	currentLine = currentLine + 1;
        if (currentLine > 5)
	    currentLine = currentLine % 6;
	if (message.length() > 22)
            putToLCD(message.substring(22));
	LCD.updateLCD();
    }
}
