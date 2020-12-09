namespace blueprint
{
    YGSize measureTextNode(YGNodeRef node, float width, YGMeasureMode, float, YGMeasureMode)
    {
        auto context = reinterpret_cast<TextShadowView*> (YGNodeGetContext (node));
        jassert (context != nullptr);

        auto view = dynamic_cast<TextView*> (context->getAssociatedView());
        jassert (view != nullptr);

        // TODO: This is a bit of an oversimplification. We have a YGMeasureMode which
        // is one of three things, "undefined", "exact", or "at-most." Here we're kind of
        // just ignoring that, and in cases like `white-space: nowrap;` we want to ignore it,
        // but it would probably be good to get specific for each case.
        // See https://github.com/facebook/yoga/pull/576/files
        const auto tl = view->getTextLayout (width);

        return { tl.getWidth(), tl.getHeight()  };
    }
}
