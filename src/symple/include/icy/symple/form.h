///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/symple/command.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


class Symple_API Form;


/// Base implementation for form pages, sections and fields.
class Symple_API FormField;
class Symple_API FormElement
{
public:
    FormElement();
    FormElement(json::Value& root, std::string_view type = "",
                std::string_view id = "", std::string_view label = "");
    FormElement(const FormElement& r);
    FormElement& operator=(const FormElement& r);
    virtual ~FormElement();

    [[nodiscard]] std::string type() const;
    [[nodiscard]] std::string id() const;
    [[nodiscard]] std::string label() const;

    /// Possible "type" values
    ///      page, section, text, text-multi,
    ///      list, list-multi, checkbox, media, custom
    void setType(std::string_view type);


    void setId(std::string_view id);
    void setLabel(std::string_view text);
    void setHint(std::string_view text);

    /// Sets and optional validation error message.
    void setError(std::string_view error);

    FormElement addPage(std::string_view id = "",
                        std::string_view label = "");
    FormElement addSection(std::string_view id = "",
                           std::string_view label = "");
    FormField addField(std::string_view type, std::string_view id = "",
                       std::string_view label = "");

    FormField getField(std::string_view id, bool partial = false);
    bool getField(std::string_view id, FormField& field,
                  bool partial = false);

    /// Returns true if the given Address matches any of the
    /// internal form element IDs.
    /// If the partial flag is set then substring matches
    /// will be counted.
    bool hasField(std::string_view id, bool partial = false);

    /// Live fields or elements are used to submit partial
    /// sections a form, without sending the entire form.
    void setLive(bool flag);

    /// Returns true if this field is live, meaning the
    /// form-processing entity should auto-update this
    /// field's value whenever it changes.
    [[nodiscard]] bool live() const;

    /// Clears child elements matching the given ID.
    bool clearElements(std::string_view id, bool partial = false);

    /// Clear the entire form.
    void clear();

    /// Returns true if the form element is valid.
    [[nodiscard]] bool valid() const;

    /// Returns the number of child elements.
    int numElements();

    /// Returns true if any fields have errors.
    bool hasErrors();

    /// Returns true if the form has multiple pages.
    bool hasPages();

    [[nodiscard]] json::Value& root() const;

protected:
    /// The root pointer is just a reference to
    /// the externally managed JSON value memory.
    json::Value* _root;
};


class Symple_API Form : public FormElement
{
public:
    Form();
    Form(json::Value& root);
    Form(Command& root);
    virtual ~Form();

    bool valid();

    [[nodiscard]] std::string action() const;
    [[nodiscard]] bool partial() const;

    /// Possible "action" values
    /// `form` The form-processing entity is asking the form-submitting entity
    /// to complete a form.
    /// `submit` The form-submitting entity is submitting data to the
    /// form-processing entity.
    /// `cancel` The form-submitting entity has cancelled submission of data to
    /// the form-processing entity.
    /// `result` The form-processing entity is returning data to the
    /// form-submitting entity.
    void setAction(std::string_view action);

    /// Notifies the form is a partial section of the form.
    /// This is used for transmitting and updating live
    /// fields such as for auto-complete.
    void setPartial(bool flag);
};


class Symple_API FormField : public FormElement
{
public:
    FormField();
    FormField(json::Value& root, std::string_view type = "",
              std::string_view id = "", std::string_view label = "");
    virtual ~FormField();

    /// Adds an option for list based fields.
    void addOption(std::string_view key, std::string_view value);
    void addOption(std::string_view value);

    /// Sets the value clearing all other values.
    void setValue(std::string_view value);
    void setValue(int value);
    void setValue(double value);
    void setValue(bool value);

    /// Appends a value to the value array.
    void addValue(std::string_view value);
    void addValue(int value);
    void addValue(double value);
    void addValue(bool value);

    /// Returns a JSON array of all values.
    json::Value& values();

    /// Returns the first value.
    /// Most formats (except multi) only
    /// require single value.
    [[nodiscard]] std::string value() const;
    [[nodiscard]] int intValue() const;
    [[nodiscard]] double doubleValue() const;
    [[nodiscard]] bool boolValue() const;
};


} // namespace smpl
} // namespace icy


/// @\}
