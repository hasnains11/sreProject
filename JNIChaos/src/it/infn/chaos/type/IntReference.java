/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package it.infn.chaos.type;

/**
 *
 * @author bisegni
 */
public class IntReference {
    int intValue=0;
    
    public int getValue(){
        return intValue;
    }
    
    public void setValue(int i) {
    	intValue = i;
    }
}