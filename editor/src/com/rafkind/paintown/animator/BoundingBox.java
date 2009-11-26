package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;

public class BoundingBox{

	// Basic character info
	public int x1;
	public int y1;
	public int x2;
	public int y2;

	public BoundingBox( int x1, int y1, int x2, int y2){
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}

	
	public boolean empty(){
		return x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0;
	}
	
	public void render( Graphics g, int x, int y ){
		Color color = g.getColor();

        if (x1 < x2 && y1 < y2){
            g.setColor(new Color(255,255,255));
            g.drawRect(x + x1, y + y1, x2-x1, y2-y1);
        } else {
            g.setColor(new Color(192, 84, 130));
            g.drawRect(x + Math.min(x1, x2), y + Math.min(y1, y2), Math.abs(x2 - x1), Math.abs(y2 - y1));
        }
		g.setColor(color);
	}
}
