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
    
    public void initDefaultCommand()
    {
        // Set the default command for a subsystem here.
    	//setDefaultCommand(new MySpecialCommand());
    	setDefaultCommand(new LEDManager());
    }
}
