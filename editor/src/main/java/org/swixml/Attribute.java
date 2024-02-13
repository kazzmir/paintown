package org.swixml;

import org.w3c.dom.Attr;
import org.w3c.dom.Element;

/**
 * Class for store attribute info.
 * 
 * @author <a href="mailto:alex73mail@gmail.com">Alex Buloichik</a>
 */
public class Attribute {
    /** Attribute name. */
    protected final String name;

    /** Attribute value. */
    protected String value;

    /** Constructor. */
    private Attribute(Attr attr) {
        name = attr.getName();
        value = attr.getValue();
    }

    /** Constructor. */
    public Attribute(final String name, final String value) {
        this.name = name;
        this.value = value;
    }

    public String getName() {
        return name;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    /**
     * Create class instance by name from element.
     * 
     * @param elem
     *            element for find attribute
     * @param attrName
     *            attribute name
     * @return object or null if attribute doesn't declared in element
     */
    public static Attribute getAttribute(Element elem, String attrName) {
        Attr attrNode = elem.getAttributeNode(attrName);
        return attrNode != null ? new Attribute(attrNode) : null;
    }

    /**
     * Get attribute value from element.
     * 
     * @param elem
     *            element for find attribute
     * @param attrName
     *            attribute name
     * @return attribute value or null if attribute doesn't declared in element
     */
    public static String getAttributeValue(Element elem, String attrName) {
        if (elem.getAttributeNode(attrName) != null) {
            return elem.getAttribute(attrName);
        } else {
            return null;
        }
    }
}
