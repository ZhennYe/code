/*
 * 
 */

#include <math.h>
#include <default_gui_model.h>

class LIF : public DefaultGUIModel
{
    public:
    
        LIF(void);
        virtual ~LIF(void);
        
        void execute(void);
        
    protected:
    
        void update(DefaultGUIModel::update_flags_t);
    
    private:
    
        double I_offset;
        double V;
        double steps, period;
};
