///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/symple/command.h"
#include "scy/symple/symple.h"


namespace scy {
namespace smpl {


class Symple_API Form;


/// Base implementation for form pages, sections and fields.
class Symple_API FormField;
class Symple_API FormElement
{
public:
    FormElement();
    FormElement(json::value& root, const std::string& type = "",
                const std::string& id = "", const std::string& label = "");
    FormElement(const FormElement& r);
    FormElement& operator=(const FormElement& r);
    virtual ~FormElement();

    [[nodiscard]] std::string type() const;
    [[nodiscard]] std::string id() const;
    [[nodiscard]] std::string label() const;

    /// Possible "type" values
    ///      page, section, text, text-multi,
    ///      list, list-multi, checkbox, media, custom
    void setType(const std::string& type);


    void setId(const std::string& id);
    void setLabel(const std::string& text);
    void setHint(const std::string& text);

    /// Sets and optional validation error message.
    void setError(const std::string& error);

    FormElement addPage(const std::string& id = "",
                        const std::string& label = "");
    FormElement addSection(const std::string& id = "",
                           const std::string& label = "");
    FormField addField(const std::string& type, const std::string& id = "",
                       const std::string& label = "");

    FormField getField(const std::string& id, bool partial = false);
    bool getField(const std::string& id, FormField& field,
                  bool partial = false);

    /// Returns true if the given Address matches any of the
    /// internal form element IDs.
    /// If the partial flag is set then substring matches
    /// will be counted.
    bool hasField(const std::string& id, bool partial = false);

    /// Live fields or elements are used to submit partial
    /// sections a form, without sending the entire form.
    void setLive(bool flag);

    /// Returns true if this field is live, meaning the
    /// form-processing entity should auto-update this
    /// field's value whenever it changes.
    [[nodiscard]] bool live() const;

    /// Clears child elements matching the given ID.
    bool clearElements(const std::string& id, bool partial = false);

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

    [[nodiscard]] json::value& root() const;

protected:
    /// The root pointer is just a reference to
    /// the externally managed JSON value memory.
    json::value* _root;
};


class Symple_API Form : public FormElement
{
public:
    Form();
    Form(json::value& root);
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
    void setAction(const std::string& action);

    /// Notifies the form is a partial section of the form.
    /// This is used for transmitting and updating live
    /// fields such as for auto-complete.
    void setPartial(bool flag);
};


class Symple_API FormField : public FormElement
{
public:
    FormField();
    FormField(json::value& root, const std::string& type = "",
              const std::string& id = "", const std::string& label = "");
    virtual ~FormField();

    /// Adds an option for list based fields.
    void addOption(const std::string& key, const std::string& value);
    void addOption(const std::string& value);

    /// Sets the value clearing all other values.
    void setValue(const std::string& value);
    void setValue(int value);
    void setValue(double value);
    void setValue(bool value);

    /// Appends a value to the value array.
    void addValue(const std::string& value);
    void addValue(int value);
    void addValue(double value);
    void addValue(bool value);

    /// Returns a JSON array of all values.
    json::value& values();

    /// Returns the first value.
    /// Most formats (except multi) only
    /// require single value.
    [[nodiscard]] std::string value() const;
    [[nodiscard]] int intValue() const;
    [[nodiscard]] double doubleValue() const;
    [[nodiscard]] bool boolValue() const;
};


} // namespace smpl
} // namespace scy


/// @\}
