/**
 * About kassert and ktry:
 * 
 * kassert performs like ASSERT.h, running the following command when 
 * assertion is not true. This is used for debugging and with defining
 * NDEBUG it will have no effect.
 * 
 * ktry will try to catch when the equation is true, and jump to the
 * label. With NDEBUG unset, it can printout where assertion is happening.
 */

#ifndef __KASSERT__
#define __KASSERT__

#include <linux/printk.h>

/**
 * kassert -   assertion for debugging check
 * @assertion: expression for asserting.
 * @error_handler: error handler when assertion failed.
 */
#ifndef NDEBUG
#define kassert(assertion, error_handler) \
    if(!assertion){ printk(KERN_CRIT "Assertion failed:"#assertion "\n"); \
     error_handler; }
#else 
#define kassert(assertion, error_handler) ;;
#endif /* NDEBUG */

/**
 * ktry - try-catch macro
 * @try: expression for catching, will be catched if true.
 * @catch: handler when catched.
 */
#ifndef NDEBUG
#define ktry(trying, catcher) \
    if(trying){ printk(KERN_INFO "%s:Line %d Catched:" #trying ,__FILE__, \
    __LINE__); goto catcher ; }
#else 
#define ktry(trying, catcher) \
    if(trying){ goto catcher ; }
#endif /* NDEBUG */

#endif /* __KASSERT__ */ 
