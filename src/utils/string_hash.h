#ifndef STRING_HASH_H_
#define STRING_HASH_H_


namespace __gnu_cxx {
/**
        Explicit template specialization of hash of a string class,
        which just uses the internal char* representation as a wrapper.
 */
template <>
struct hash<std::string> {
        size_t operator() (const std::string& x) const {
                return hash<const char*>()(x.c_str());
	// hash<const char*> already exists
        }
};
}



#endif /*STRING_HASH_H_*/
