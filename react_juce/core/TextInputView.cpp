/*
==============================================================================

  TextInputView.cpp
  Created: 20 Jan 2021 10:30pm

==============================================================================
*/

#include "TextInputView.h"
#include "TextView.h"

namespace reactjuce
{
    namespace detail
    {
        static juce::var makeInputEventObject(const juce::String &value)
        {
            juce::DynamicObject::Ptr obj = new juce::DynamicObject();
            obj->setProperty("value", value);
            return obj.get();
        }

        static juce::var makeChangeEventObject(const juce::String &value)
        {
            juce::DynamicObject::Ptr obj = new juce::DynamicObject();
            obj->setProperty("value", value);
            return obj.get();
        }
    }

    void TextInputView::TextInput::setControlledValue(const juce::String &value)
    {
        insertedAsControlledValue = true;
        setText(value);
    }

    void TextInputView::TextInput::setMaxLength(int maxLen)
    {
        maxLength = maxLen;
        setInputRestrictions(maxLen);
    }

    void TextInputView::TextInput::setPlaceholderText(const juce::String &text)
    {
        placeholderText = text;
        setTextToShowWhenEmpty(placeholderText, placeholderColour);
    }

    void TextInputView::TextInput::setPlaceholderColour(const juce::Colour &colourToUse)
    {
        placeholderColour = colourToUse;
        setTextToShowWhenEmpty(placeholderText, placeholderColour);
    }

    void TextInputView::TextInput::insertTextAtCaret(const juce::String &textToInsert)
    {
        const juce::String currentValue = getText();
        juce::TextEditor::insertTextAtCaret(textToInsert);
        const juce::String newValue = getText();
        if (currentValue == newValue)
        {
            return;
        }

        // Invoke JavaScript's `input` event.
        if (props.contains(TextInputView::onInputProp) && props[TextInputView::onInputProp].isMethod())
        {
            std::array<juce::var, 1> args{{detail::makeInputEventObject(newValue)}};
            juce::var::NativeFunctionArgs nfArgs(juce::var(), args.data(), static_cast<int>(args.size()));
            std::invoke(props[TextInputView::onInputProp].getNativeFunction(), nfArgs);
        }

        dirty = true;

        if (controlled && !insertedAsControlledValue)
        {
            undo();
        }
        insertedAsControlledValue = false;
    }

    //==============================================================================

    void TextInputView::TextInput::textEditorReturnKeyPressed(juce::TextEditor &)
    {
        invokeChangeEventIfNeeded();
    }

    void TextInputView::TextInput::textEditorFocusLost(juce::TextEditor &)
    {
        invokeChangeEventIfNeeded();
    }

    void TextInputView::TextInput::invokeChangeEventIfNeeded()
    {
        if (dirty)
        {
            // Invoke JavaScript's `change` event.
            if (props.contains(TextInputView::onChangeProp) && props[TextInputView::onChangeProp].isMethod())
            {
                std::array<juce::var, 1> args{{detail::makeChangeEventObject(getText())}};
                juce::var::NativeFunctionArgs nfArgs(juce::var(), args.data(), static_cast<int>(args.size()));
                std::invoke(props[TextInputView::onChangeProp].getNativeFunction(), nfArgs);
            }
            dirty = false;
        }
    }

    //==============================================================================

    void TextInputView::setProperty(const juce::Identifier &name, const juce::var &value)
    {
        View::setProperty(name, value);
        if (name == valueProp)
        {
            if (!value.isString())
                throw std::invalid_argument("Invalid prop value. Prop \'value\' must be a string.");

            textInput.setControlled(true);
            textInput.setControlledValue(value);
        }

        if (name == placeholderProp)
        {
            if (!value.isString())
                throw std::invalid_argument("Invalid prop value. Prop \'placeholder\' must be a string.");

            textInput.setPlaceholderText(value);
        }

        if (name == placeholderColorProp)
        {
            if (!value.isString())
                throw std::invalid_argument("Invalid prop value. Prop \'placeholder-color\' must be a color string.");

            const juce::String hexPlaceholderColor = value;
            const juce::Colour placeholderColor = juce::Colour::fromString(hexPlaceholderColor);
            textInput.setPlaceholderColour(placeholderColor);
        }

        if (name == maxlengthProp)
        {
            if (!value.isDouble())
              throw std::invalid_argument("Invalid prop value. Prop \'maxlength\' must be a number.");

            textInput.setMaxLength(value);
        }

        if (name == readonly)
        {
            textInput.setReadOnly(value);
        }

        textInput.applyFontToAllText(getFont());

        const juce::String hexColor = props.getWithDefault(colorProp, "ff000000");
        const juce::Colour colour = juce::Colour::fromString(hexColor);
        textInput.applyColourToAllText(colour);

        const int just = props.getWithDefault(justificationProp, 1);
        textInput.setJustification(just);

        if (isTextEditorColorProp(name))
            setTextEditorColorProp(name, value);
    }

    void TextInputView::resized()
    {
        View::resized();
        textInput.setBounds(getLocalBounds());
    }

    juce::Font TextInputView::getFont() const
    {
        return TextView::getFont(props);
    }

    bool TextInputView::isTextEditorColorProp(const juce::Identifier &textEditorColorProp)
    {
        const auto it = textEditorColourIdsByProp.find(textEditorColorProp);
        return it != textEditorColourIdsByProp.end();
    }

    void TextInputView::setTextEditorColorProp(const juce::Identifier &textEditorColorProp, const juce::var &value)
    {
        if (!value.isString())
        {
            const std::string propName = textEditorColorProp.toString().toStdString();
            const std::string message = "Invalid prop value. Prop \'" + propName + "\' must be a color string.";
            throw std::invalid_argument(message);
        }

        const juce::String hexColor = value;
        const juce::Colour color = juce::Colour::fromString(hexColor);
        textInput.setColour(textEditorColourIdsByProp.at(textEditorColorProp), color);
    }

    const std::map<juce::Identifier, int> TextInputView::textEditorColourIdsByProp = {
        { backgroundColorProp,      juce::TextEditor::ColourIds::backgroundColourId },
        { outlineColorProp,         juce::TextEditor::ColourIds::outlineColourId },
        { focusedOutlineColorProp,  juce::TextEditor::ColourIds::focusedOutlineColourId },
        { highlightedTextColorProp, juce::TextEditor::ColourIds::highlightedTextColourId },
        { highlightColorProp,       juce::TextEditor::ColourIds::highlightColourId },
        { caretColorProp,           juce::CaretComponent::ColourIds::caretColourId }
    };
}
