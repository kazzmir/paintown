package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.Player;

public class SpecialPanel extends JPanel
{
	protected Player player;
	protected JPanel child;
	protected JTextField text;
	public SpecialPanel(JPanel c, JTextField t)
	{
		child = c;
		text = t;
		add(child);
	}
	public SpecialPanel(JPanel c, JTextField t, Player p)
	{
		child = c;
		text = t;
		add(child);
		player = p;
	}
	public JTextField getTextBox(){return text;}
	public Player getPlayer(){return player;}
}
