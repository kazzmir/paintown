#ifndef _rtech1_input_source_h
#define _rtech1_input_source_h

#include <vector>

/* this class should abstract over actual devices (keyboard, joystick)
 * and return a list of events on request.
 */

class InputSource{
public:
    InputSource(bool default_);
    /* keyboard specifies the configuration to use.
     * joystick specifies the configuration and which physical joystick to use.
     */
    InputSource(const std::vector<int> & keyboard, const std::vector<int> & joystick);
    InputSource(const InputSource & copy);
    virtual ~InputSource();

    /* Creates a copy of this input source with the keyboard/joystick added
     * to the current list
     */
    InputSource addKeyboard(int keyboard);
    InputSource addJoystick(int joystick);

    InputSource & operator=(const InputSource &);

    virtual bool useKeyboard() const;
    virtual const std::vector<int> & getKeyboard() const;
    virtual bool useJoystick() const;
    virtual const std::vector<int> & getJoystick() const;

private:
    std::vector<int> keyboard;
    std::vector<int> joystick;
};

#endif
