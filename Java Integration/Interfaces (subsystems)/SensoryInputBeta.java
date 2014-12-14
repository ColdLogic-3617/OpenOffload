/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.cold.logic.interfaces;

import edu.gappleto.common.Logger;

import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 *
 * @author Gabe
 */
public class SensoryInputBeta extends Subsystem {
    // Put methods for controlling this subsystem
    // here. Call these from Commands.
    
    private int hotness;
    private DriverStation station = DriverStation.getInstance();

    public void initDefaultCommand() {
        // Set the default command for a subsystem here.
        //setDefaultCommand(new MySpecialCommand());
    }
    
    public void updateVision()  {
        int prev = hotness;
        hotness = SmartDashboard.getNumber("Hotness");
        if (prev != hotness)    {
            if (station.isAutonomous()) {
                if (prev == Var.autonomousHotGoals)
                    Logger.printlnBeta("The goal is no longer hot.");
                else
                    Logger.printlnBeta("The goal is now hot.");
            } else    {
                if (prev == Var.autonomousHotGoals)
                    Logger.printlnBeta("You are not aimed at the goal.");
                else
                    Logger.printlnBeta("You are now aimed at the goal.");
            }
        }
    }
    
    public int getVision() {
        updateVision();
        return hotness;        
    }
    
    public boolean isHot()  {
        updateVision();
        return (hotness == Var.autonomousHotGoals)
    }
}
