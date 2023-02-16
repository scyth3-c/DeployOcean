#include "parameter_proccess.h"


HTTP_QUERY::HTTP_QUERY() {}
HTTP_QUERY::~HTTP_QUERY() {}


string HTTP_QUERY::x_www_form_urlencoded(string &target, string plain)
{
    string params{};
    std::cout << std::flush;
    for (size_t ti = target.length() - 1; ti > 0; ti--)
    {
        if (target[ti] == ' ' || target[ti] == char(13))
            break;
        params += target[ti];
    }
    std::reverse(params.begin(), params.end());

    fflush(stdin);
    return plain + params;
}

string HTTP_QUERY::selectPerType(string &target, string &conten_type, bool &init)
{
    std::cout << std::flush;
    if (conten_type == X_WWW_FORM)
    {
        init = true;
        return x_www_form_urlencoded(target);
    }
    else if (conten_type == PLAIN_TEXT)
    {
        init = true;
        return x_www_form_urlencoded(target, "data=");
    }
    else
    {
        return get_params(target, init);
    }
}

string HTTP_QUERY::route_refactor_params(string _target)
{
    bool init = false;
    string content_type = findContenType(_target);
    string content_type_value = trim(std::move(content_type));
    std::cout << std::flush;
    if(content_type != STR_ERR) {
        route = make_shared<string>(selectPerType(_target, content_type_value , init));
        return init ? *route : NOT_PARAMS;
    } else{
        return NOT_PARAMS;
    }
}

std::pair<string, string> HTTP_QUERY::route_refactor(string target)
{

    size_t size = target.size();
    std::pair<string, string> route;
    bool init = false;

    std::cout<<std::flush;

    for (auto &it : target)
    {
        if (it == char(32) || it == '/')
        {
            break;
        }
        else
        {
            route.first += it;
            std::cout<<std::flush;
        }
    }

    std::cout<<std::flush;
    for (size_t ui = 0; ui < size; ui++)
    {

        if (init)
        {
            if (target[ui] == '?' || target[ui] == 'H')
            {
                break;
            }
            if (target[ui] != ' ')
            {
                std::cout<<std::flush;
                route.second += target[ui];
                std::cin.clear();
            }
        }
        else if (target[ui + 1] == '/')
        {
            init = true;
        }
    }

    fflush(stdin);
    return route;
}


string HTTP_QUERY::findContenType(string text)
{
    std::string content_type = "";
    size_t index = text.find("Content-Type: ");

    std::cout << std::flush;
    if (index != std::string::npos) {
        index += 13; 
        while (text[index] != char(13)) {
            content_type += text[index];
            index++;
        }
        return content_type;
    } else {
        return STR_ERR;
    }   
}


string HTTP_QUERY::get_params(string &target, bool &init)
{
    string *params = new string("");
    std::cout << std::flush;
    for (size_t ui = 3; ui < target.size(); ui++)
    {
        if (init)
        {
            if (target[ui] == 'H' || target[ui] == ' ')
                break;
            *params += target[ui];
        }
        else if (target[ui] == '?' && ui <= max_iterator)
        {
            init = true;
        }
    }
    fflush(stdin);
    return *params;
}

string HTTP_QUERY::trim(string target){
    target.erase(std::remove_if(target.begin(), target.end(), ::isspace), target.end());
    return target;
}