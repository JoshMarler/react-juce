import React, { Component, PropsWithChildren, ReactElement } from "react";

import { ViewInstance } from "../lib/Backend";
import { ScrollView, ScrollEvent } from "./ScrollView";

import invariant from "invariant";

type ListViewState = {
  scrollTopPosition: number;
  width: number;
  height: number;
};

type VirtualPositions = {
  innerHeight: number;
  startIndex: number;
  endIndex: number;
};

/**
 * ScrollParams is passed to ListView's scrollToIndex
 * function.
 *
 * @property index - The index of the item in the ListView's data
 *                   source to scroll to.
 *
 * @property offset - A normalised value between 0.0 and 1.0 which
 *                    will offset the position of the ListView item
 *                    (specified by index) from the top of the ListView.
 *                    i.e. offset == 0.5 positions the item at index in
 *                    the center of the ListView.
 */
export interface ScrollParams {
  index: number;
  offset: number;
}

/**
 * Prop type for ListView.
 *
 * @property data       - Array of objects to use when populating ListView items
 *                        via the renderItem callback.
 *
 * @property renderItem - Callback to render a ListView item given an element
 *                        from the supplied data source.
 *
 * @property itemHeight - Fixed height in pixels for a single item in the
 *                        ListView.
 */
export interface ListViewProps {
  data: any[];
  renderItem: (any) => ReactElement;
  itemHeight: number;
}

/**
 * A lightweight "virtualised list" implementation that allows for
 * the efficient rendering of a large number of objects within a
 * ScrollView. ListView supports the full set of ScrollViewProps
 * for styling etc.
 *
 * Note, at present ListView requires the user to specify a fixed
 * itemHeight property in order to calculate which items to render
 * within the list based on scroll position. This may change in
 * future versions.
 *
 * @example
 *
 * function getListViewItems() {
 *   let items = [];
 *
 *   for (let i = 0; i < 5000; ++i) {
 *     const name     = "Item " + i;
 *     const category = i % 2 ? "A" : "B";
 *
 *     items.push({id: i, name: name, category: category});
 *   }
 *
 *   return items;
 * }
 *
 * <ListView
 *   height="100%"
 *   width="50%"
 *   overflow="hidden"
 *   scroll-on-drag={true}
 *   data={getListViewItems()}
 *   renderItem={(item, index, props) => <Item name={item.name} category={item.category} {...props}/>}
 *   itemHeight={50}
 * />
 *
 */
export class ListView extends Component<
  PropsWithChildren<ListViewProps | any>,
  ListViewState
> {
  private _ref: React.RefObject<ViewInstance>;

  constructor(props: PropsWithChildren<ListViewProps | any>) {
    super(props);
    this._ref = React.createRef();

    this._onMeasure = this._onMeasure.bind(this);
    this._calculateVirtualPositions = this._calculateVirtualPositions.bind(
      this
    );
    this._setScrollTopPosition = this._setScrollTopPosition.bind(this);
    this.scrollToIndex = this.scrollToIndex.bind(this);

    this.state = {
      scrollTopPosition: 0,
      width: 0,
      height: 0,
    };
  }

  _onMeasure(e: any): void {
    this.setState({
      width: e.width,
      height: e.height,
    });
  }

  _calculateVirtualPositions(): VirtualPositions {
    const totalItems = this.props.data.length;
    const innerHeight = this.props.itemHeight * totalItems;

    invariant(
      this.props.itemHeight > 0,
      "Zero or negative itemHeight passed to ListView"
    );

    // Pad num rendered items by 1 so we always render the same number of items
    // which allows us to use a fixed key value on each item in the list.
    const numItems = Math.floor(this.state.height / this.props.itemHeight) + 1;

    const startIndex = Math.floor(
      this.state.scrollTopPosition / this.props.itemHeight
    );

    const endIndex = Math.min(totalItems - 1, startIndex + numItems);

    return {
      innerHeight: innerHeight,
      startIndex: startIndex,
      endIndex: endIndex,
    };
  }

  _setScrollTopPosition(e: ScrollEvent): void {
    this.setState({
      scrollTopPosition: e.scrollTop,
    });

    if (typeof this.props.onScroll === "function") {
      this.props.onScroll(e);
    }
  }

  scrollToIndex(scrollParams: ScrollParams) {
    invariant(
      scrollParams.index >= 0 && scrollParams.index <= this.props.data.length,
      "scrollParams.index must be between 0 and props.data.length"
    );

    invariant(
      scrollParams.offset >= 0.0 && scrollParams.offset <= 1.0,
      "scrollParams.offset must be normalised between 0.0 and 1.0"
    );

    const numItems = Math.floor(this.state.height / this.props.itemHeight);

    const xPos = 0;
    let yPos =
      this.props.itemHeight * scrollParams.index -
      this.props.itemHeight * numItems * scrollParams.offset;

    const scrollViewInstance = this._ref ? this._ref.current : null;

    if (scrollViewInstance) {
      //@ts-ignore
      scrollViewInstance.scrollToPosition(xPos, yPos);
    }
  }

  render() {
    const positions: VirtualPositions = this._calculateVirtualPositions();
    const items: any = [];

    // List items must have a key but we ensure the key remains fixed here
    // so that we simply re-render the existing views/components rather than
    // replacing them. This stops issues occuring when dynamically adding and removing
    // components inside a juce::Viewport.
    if (this.state.height > 0) {
      for (let i = positions.startIndex; i <= positions.endIndex; ++i) {
        items.push(
          this.props.renderItem(this.props.data[i], i, {
            position: "absolute",
            top: this.props.itemHeight * i,
            key: positions.endIndex - i,
          })
        );
      }
    }

    return (
      <ScrollView
        {...this.props}
        onMeasure={this._onMeasure}
        onScroll={this._setScrollTopPosition}
        viewRef={this._ref}
      >
        <ScrollView.ContentView
          {...styles.scrollViewContent}
          height={positions.innerHeight}
        >
          {items}
        </ScrollView.ContentView>
      </ScrollView>
    );
  }
}

const styles = {
  scrollViewContent: {
    flexDirection: "column",
    flex: 1.0,
    flexShrink: 0.0,
  },
};
