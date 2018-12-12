import React, { Component } from 'react';

class App extends Component {
  constructor(props) {
    super(props);

    this._timerId = null;

    this.state = {
      date: new Date(),
    };
  }

  tick() {
    // this.setState({
    //   date: new Date(),
    // });
  }

  componentDidMount() {
    // this._timerId = setInterval(
    //   () => this.tick(),
    //   1000
    // );
  }

  componentWillUnmount() {
    // clearInterval(this._timerId);
  }

  render() {
    const styleObj = {
      'background-color': 'ff272777',
      'align-items': 'center',
      'flex': 1.0,
      'padding': 20.0,
      ...this.props.style,
    };

    const childStyleObj = {
      'background-color': '77772727',
      'flex': 1.0,
      'height': 200.0,
      'margin-horizontal': 80.0,
      'justify-content': 'center',
    };

    return (
      <div debug="1" {...styleObj}>
        <div {...childStyleObj}>
          The date today is: {this.state.date.toLocaleDateString()}
        </div>
      </div>
    );
  }
}

export default App;
