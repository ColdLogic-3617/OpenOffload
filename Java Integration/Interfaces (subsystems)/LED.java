/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.cold.logic.interfaces;

import edu.cold.logic.commands.LEDManager;
import edu.cold.logic.Var;

import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.DriverStation.Alliance;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * A class for interfacing with the Arduino's LED systems
 * @author ThePenultimateOne
 */
public class LED extends Subsystem
{
    // Put methods for controlling this subsystem
    // here. Call these from Commands.
    static private final DigitalOutput enabler = new DigitalOutput(Var.enabler),
				       team = new DigitalOutput(Var.team);
    
    static private final DriverStation station = DriverStation.getInstance();
    
    static private boolean enabled = false,
    			   red = false,
    			   autonomous = false;
    
    /**
     * Sets the Arduino's LEDs to indicate that the robot is enabled
     * @author ThePenultimateOne
     * @since Bruce (2.2.2)
    */
    public void setEnabled()
    {
	enabler.set(station.isEnabled());
    }
    
    /**
     * Sets the Arduino's LEDs to indicate which alliance the robot is a member of
     * @author ThePenultimateOne
     * @since Bruce (2.2.2)
    */
    public void setTeam()
    {
        team.set(station.getAlliance() == Alliance.kRed);
    }
    
    /**
     * Sets the Arduino's LEDs to indicate that the robot is disabled
     * @author ThePenultimateOne
     * @since Bruce (2.2.2)
    */
    static public void setDisabled()
    {
	team.set(false);
    }

    /**
     * Sets the Arduino's LEDs to indicate whether the robot is enabled
     * @author ThePenultimateOne
     * @since Bruce (2.2.2)
    */
    static public void run()
    {
    	team.set(station.isEnabled());
    }
    
    /**
     * Updates all LED-related information over the SmartDashboard network
     * @author ThePenultimateOne
     * @since OpenOffload (0.1.0) || Bruce (2.4.0)
    */
    static public void updateNetworkBeta()	{
    	if (red != (station.getAlliance() == Alliance.kRed))	{ //These checks are for bandwidth conservation
    		red = (station.getAlliance() == Alliance.kRed);
    		SmartDashboard.putBoolean("Red",red);	
    	}
    	if (enabled != (station.isEnabled))	{
    		enabled = station.isEnabled();
    		SmartDashboard.pubBoolean("Enabled",enabled);
    	}
    	if (autonomous != (station.isAutonomous()))	{
    		autonomous = (station.isAutonomous());
    		SmartDashboutd.putBoolean("Autonomous",autonomous);
    	}
    }
    
    public void initDefaultCommand()
    {
        // Set the default command for a subsystem here.
    	//setDefaultCommand(new MySpecialCommand());
    	setDefaultCommand(new LEDManager());
    }
}
