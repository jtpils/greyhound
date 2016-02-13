#pragma once

#include <functional>
#include <stdexcept>
#include <string>

#include <entwine/types/bbox.hpp>

#include "status.hpp"

static inline v8::Local<v8::String> toSymbol(
        v8::Isolate* isolate,
        const std::string& str)
{
    return v8::String::NewFromUtf8(isolate, str.c_str());
}

static inline entwine::BBox parseBBox(const v8::Local<v8::Value>& jsBBox)
{
    entwine::BBox bbox;

    try
    {
        std::string bboxStr(std::string(
                    *v8::String::Utf8Value(jsBBox->ToString())));

        Json::Reader reader;
        Json::Value rawBounds;

        reader.parse(bboxStr, rawBounds, false);

        Json::Value json;
        Json::Value& bounds(json["bounds"]);

        if (rawBounds.size() == 4)
        {
            bounds.append(rawBounds[0].asDouble());
            bounds.append(rawBounds[1].asDouble());
            bounds.append(0);
            bounds.append(rawBounds[2].asDouble());
            bounds.append(rawBounds[3].asDouble());
            bounds.append(0);

            json["is3d"] = false;
        }
        else if (rawBounds.size() == 6)
        {
            bounds.append(rawBounds[0].asDouble());
            bounds.append(rawBounds[1].asDouble());
            bounds.append(rawBounds[2].asDouble());
            bounds.append(rawBounds[3].asDouble());
            bounds.append(rawBounds[4].asDouble());
            bounds.append(rawBounds[5].asDouble());

            json["is3d"] = true;
        }
        else
        {
            throw std::runtime_error("Invalid");
        }

        bbox = entwine::BBox(json);
    }
    catch (...)
    {
        std::cout << "Invalid BBox in query." << std::endl;
    }

    return bbox;
}

class Background
{
public:
    void safe(std::function<void()> f)
    {
        try
        {
            f();
        }
        catch (const std::runtime_error& e)
        {
            status.set(500, e.what());
        }
        catch (const std::bad_alloc& ba)
        {
            status.set(500, "Bad alloc");
        }
        catch (...)
        {
            status.set(500, "Unknown error");
        }
    }

    Status status;
};

