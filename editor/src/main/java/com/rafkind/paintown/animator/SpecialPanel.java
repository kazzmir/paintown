package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import javax.swing.*;

public class SpecialPanel extends JPanel {
	protected BasicObject object;
	protected JPanel child;
	protected JTextField text;

	public SpecialPanel(JPanel c, JTextField t, BasicObject object ){
            this.setLayout(new GridBagLayout());
            child = c;
            text = t;
            GridBagConstraints constraints = new GridBagConstraints();
            constraints.gridx = 0;
            constraints.gridy = 0;
            constraints.weightx = 1;
            constraints.weighty = 1;
            constraints.fill = GridBagConstraints.BOTH;
            constraints.anchor = GridBagConstraints.NORTHWEST;
            // JButton button = new JButton("Hello world!");
            // layout.setConstraints(child, constraints);
            // layout.setConstraints(button, constraints);
            // add(new JButton("Hello world!"), constraints);
            add(child, constraints);
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
