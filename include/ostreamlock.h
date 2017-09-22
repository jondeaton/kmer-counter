/**
 * Provides a locking mechanism that allows us to lock down access to stream references.
 * Author: Jerry Cain
 */

#include <ostream>

std::ostream& oslock(std::ostream& os);
std::ostream& osunlock(std::ostream& os);
