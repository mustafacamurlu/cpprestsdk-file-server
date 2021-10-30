#ifndef _JWT_VALIDATOR_HPP_
#define _JWT_VALIDATOR_HPP_

#include <string>

using namespace std;

namespace mstfcmrl
{

    class jwt_validator
    {
        string raw_jwks;
        string pub_key_uri;

        public:
            enum class jwt_validator_error_codes
            {
                OK,
                INTERNAL_ERROR,
                VERIFICATION_FALURE
            };

            jwt_validator(string public_key_uri);
            jwt_validator_error_codes validate(string& jwt_token);
    };
}

#endif /* _JWT_VALIDATOR_HPP_ */