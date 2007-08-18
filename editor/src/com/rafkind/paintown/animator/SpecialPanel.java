package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.Player;

public class SpecialPanel extends JPanel
{
	protected CharacterStats player;
	protected JPanel child;
	protected JTextField text;

	public SpecialPanel(JPanel c, JTextField t, CharacterStats character ){
		child = c;
		text = t;
		add(child);
		player = character;
	}

	public JTextField getTextBox(){return text;}
	public CharacterStats getPlayer(){return player;}
}
