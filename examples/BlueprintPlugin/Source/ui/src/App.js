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
      'background-color': 'blue',
      ...this.props.style,
    };

    return (
      <div style={styleObj}>
        <div style={{'background-color': 'rgba(200, 200, 100, 0.5)', 'color': 'white', 'width': '50%'}}>
        </div>
      </div>
    );
  }
}

export default App;
