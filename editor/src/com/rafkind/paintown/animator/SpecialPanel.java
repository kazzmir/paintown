package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import javax.swing.*;

public class SpecialPanel extends JPanel
{
	protected JPanel child;
	protected JTextField text;
	public SpecialPanel(JPanel c, JTextField t)
	{
		child = c;
		text = t;
		add(child);
	}
	public JTextField getTextBox(){return text;}
}
