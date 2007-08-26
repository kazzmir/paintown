package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import javax.swing.*;

public class SpecialPanel extends JPanel {
	protected BasicObject object;
	protected JPanel child;
	protected JTextField text;

	public SpecialPanel(JPanel c, JTextField t, BasicObject object ){
		child = c;
		text = t;
		add(child);
		this.object = object;
	}

	/* used to update the panel name */
	public JTextField getTextBox(){
		return text;
	}

	public BasicObject getObject(){
		return object;
	}
}
