///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/form.h"
#include "icy/util.h"

#include <stdexcept>


namespace icy {
namespace smpl {


Form::Form()
{
}


Form::Form(Command& root)
    : FormElement(root["form"])
{
    root["form"]["type"] = "form";
}


Form::Form(json::Value& root)
    : FormElement(root)
{
    root["type"] = "form";
}


Form::~Form()
{
}


std::string Form::action() const
{
    return root().value("action", "form");
}


bool Form::partial() const
{
    return root()["partial"].get<bool>();
}


void Form::setPartial(bool flag)
{
    root()["partial"] = flag;
}


void Form::setAction(std::string_view action)
{
    if (action != "form" && action != "submit" && action != "cancel" && action != "result")
        throw std::invalid_argument("Invalid form action: " + std::string(action));
    root()["action"] = std::string(action);
}


bool Form::valid()
{
    return FormElement::valid() && root().size() > 0 && !hasErrors();
}


// ---------------------------------------------------------------------
// Form Element
//
FormElement::FormElement()
    : _root(nullptr)
{
}


FormElement::FormElement(json::Value& root, std::string_view type,
                         std::string_view id, std::string_view label)
    : _root(&root)
{
    if (!type.empty())
        root["type"] = std::string(type);
    if (!id.empty())
        root["id"] = std::string(id);
    if (!label.empty())
        root["label"] = std::string(label);
}


FormElement::FormElement(const FormElement& r)
    : _root(r._root)
{
}


FormElement& FormElement::operator=(const FormElement& r)
{
    _root = r._root;
    return *this;
}


FormElement::~FormElement()
{
}


std::string FormElement::type() const
{
    return root()["type"].get<std::string>();
}


std::string FormElement::id() const
{
    return root()["id"].get<std::string>();
}


std::string FormElement::label() const
{
    return root()["label"].get<std::string>();
}


void FormElement::setType(std::string_view type)
{
    root()["type"] = std::string(type);
}


void FormElement::setId(std::string_view id)
{
    root()["id"] = std::string(id);
}


void FormElement::setLabel(std::string_view text)
{
    root()["label"] = std::string(text);
}


void FormElement::setHint(std::string_view text)
{
    root()["hint"] = std::string(text);
}


void FormElement::setError(std::string_view error)
{
    root()["error"] = std::string(error);
}


FormElement FormElement::addPage(std::string_view id,
                                 std::string_view label)
{
    return FormElement(root()["elements"][root()["elements"].size()], "page", id, label);
}


FormElement FormElement::addSection(std::string_view id,
                                    std::string_view label)
{
    return FormElement(root()["elements"][root()["elements"].size()], "section", id, label);
}


FormField FormElement::addField(std::string_view type, std::string_view id,
                                std::string_view label)
{
    if (type != "text" && type != "text-multi" && type != "list" &&
        type != "list-multi" && type != "boolean" && type != "number" &&
        type != "media" && type != "date" && type != "time" &&
        type != "datetime" && type != "horizontal-set" && type != "custom")
        throw std::invalid_argument("Invalid form field type: " + std::string(type));

    return FormField(root()["elements"][root()["elements"].size()], type, id,
                     label);
}


void FormElement::clear()
{
    root().clear();
}


bool FormElement::valid() const
{
    return _root != nullptr;
}


int FormElement::numElements()
{
    return root()["elements"].size();
}


bool FormElement::hasErrors()
{
    return json::hasNestedKey(root(), "error");
}


bool FormElement::live() const
{
    return root()["live"].get<bool>();
}


void FormElement::setLive(bool flag)
{
    root()["live"] = flag;
}


bool FormElement::clearElements(std::string_view id, bool partial)
{
    // json::Value& root() = section.root()();
    bool match = false;
    json::Value result;

    for (auto& [k, v] : root().items()) {

        // Filter elements
        if (k == "elements") {
            for (unsigned x = 0; x < root()["elements"].size(); x++) {
                json::Value& element = root()["elements"][x];
                std::string curID = element["id"].get<std::string>();
                if ( // element.is_object() &&
                    // element.isMember("id") &&
                    partial ? curID.find(id) != std::string::npos
                            : curID == id) {
                    LTrace("Symple form: Removing redundant: ", curID);
                    match = true;
                } else {
                    LTrace("Symple form: Keeping: ", curID);
                    result["elements"]
                        .push_back(element);
                }
            }
        }

        // Keep other members
        else
            result[k] = v;
    }

    *_root = result;
    return match;
}


bool FormElement::getField(std::string_view id, FormField& field, bool partial)
{
    return json::findNestedObjectWithProperty(root(), field._root, "id", id, partial);
}


FormField FormElement::getField(std::string_view id, bool partial)
{
    FormField field;
    json::findNestedObjectWithProperty(root(), field._root, "id", id, partial);
    return field;
}


bool FormElement::hasField(std::string_view id, bool partial)
{
    json::Value* tmp = nullptr;
    return json::findNestedObjectWithProperty(root(), tmp, "id", id, partial);
}


bool FormElement::hasPages()
{
    json::Value* tmp = nullptr;
    return json::findNestedObjectWithProperty(root(), tmp, "type", "page");
}


json::Value& FormElement::root() const
{
    if (_root == nullptr)
        throw std::runtime_error("Form root is unassigned");
    return *_root;
}


// ---------------------------------------------------------------------
// Form Field
//
FormField::FormField()
{
}


FormField::FormField(json::Value& root, std::string_view type,
                     std::string_view id, std::string_view label)
    : FormElement(root, type, id, label)
{
}


FormField::~FormField()
{
}


json::Value& FormField::values()
{
    return root()["values"];
}


std::string FormField::value() const
{
    return root()["values"][static_cast<unsigned>(0)].get<std::string>();
}


int FormField::intValue() const
{
    return util::strtoi<uint32_t>(value());
}


double FormField::doubleValue() const
{
    return util::strtoi<double>(value());
}


bool FormField::boolValue() const
{
    std::string val = value();
    return val == "1" || val == "true" || val == "on" ? true : false;
}


void FormField::addOption(std::string_view value)
{
    std::string v(value);
    root()["options"][v] = v;
}


void FormField::addOption(std::string_view key, std::string_view value)
{
    root()["options"][std::string(key)] = std::string(value);
}


void FormField::setValue(std::string_view value)
{
    root()["values"].clear();
    root()["values"].push_back(std::string(value));
}


void FormField::setValue(int value)
{
    setValue(util::itostr<int>(value));
}


void FormField::setValue(double value)
{
    setValue(util::itostr<double>(value));
}


void FormField::setValue(bool value)
{
    setValue(std::string(value ? "1" : "0"));
}


void FormField::addValue(std::string_view value)
{
    root()["values"].push_back(std::string(value));
}


void FormField::addValue(int value)
{
    addValue(util::itostr<int>(value));
}


void FormField::addValue(double value)
{
    addValue(util::itostr<double>(value));
}


void FormField::addValue(bool value)
{
    addValue(std::string(value ? "1" : "0"));
}


} // namespace smpl
} // namespace icy


/// @}
