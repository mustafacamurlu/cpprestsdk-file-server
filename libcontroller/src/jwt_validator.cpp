
#include "jwt_validator.hpp"

#include <cpprest/http_client.h>
#include "jwt-cpp/jwt.h"

using namespace std;
using namespace web;
using namespace http;
using namespace client;

mstfcmrl::jwt_validator::jwt_validator(string public_key_uri)
    :pub_key_uri(public_key_uri)
{
}

mstfcmrl::jwt_validator::jwt_validator_error_codes mstfcmrl::jwt_validator::validate(string& jwt_token)
{
    http_client client(pub_key_uri);
    http_response response = client.request(methods::GET).get();

    if (response.status_code() != 200)
    {
        return mstfcmrl::jwt_validator::jwt_validator_error_codes::INTERNAL_ERROR;
    }
    else
    {
        try
        {
            raw_jwks = response.extract_string().get();

            auto jwks = jwt::parse_jwks(raw_jwks);
            auto decoded_jwt = jwt::decode(jwt_token);
            auto issuer = decoded_jwt.get_issuer();
            auto x5c = jwks.get_jwk(decoded_jwt.get_key_id()).get_x5c_key_value();
            if (!x5c.empty() && !issuer.empty())
            {
                auto verifier = jwt::verify()
                                    .allow_algorithm(jwt::algorithm::rs256(jwt::helper::convert_base64_der_to_pem(x5c), "", "", ""))
                                    .with_issuer(issuer)
                                    .leeway(60UL);
                verifier.verify(decoded_jwt);
            }
        }
        catch(std::exception& err)
        {
            return mstfcmrl::jwt_validator::jwt_validator_error_codes::VERIFICATION_FALURE;
        }
    }

    return mstfcmrl::jwt_validator::jwt_validator_error_codes::OK;
}
