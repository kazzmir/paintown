package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;

public class MoveEvent implements AnimationEvent
{
	private int _x;
	private int _y;
	private int _z;
	
	public void move(int x, int y, int z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	
	public void interact(DrawArea area)
	{
	}
	
	public String getName()
	{
		return new String("move");
	}
	
	public JDialog getEditor()
	{
		return new JDialog();
	}
	
	public Token getToken()
	{
		Token temp = new Token("move");
		temp.addToken(new String[]{"move",Integer.toString(_x),Integer.toString(_y),Integer.toString(_z)});
		return temp;
	}
}
