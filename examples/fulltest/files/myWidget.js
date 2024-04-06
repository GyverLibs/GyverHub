class MyInput extends BaseWidget {
    static wtype = 'myinput';
    $el;

    constructor(data, renderer) {
        super(data, renderer);

        this.makeLayout({
            tag: 'div',
            children: [
                {
                    tag: 'input',
                    name: 'el',
                    inputType: 'text',
                    class: 'myinp',
                },
                {
                    tag: 'button',
                    text: 'send',
                    class: 'mybtn',
                    events: {
                        click: (e) => {
                            this.set(this.$el.value);
                        }
                    }
                }
            ]
        });

        this.update(data);
    }

    update(data) {
        super.update(data);
        if ('value' in data) this.$el.value = data.value;
    }

    static style = `
  .myinp {
      font-size: 17px;
      border: 2px solid black;
    }
    .mybtn {
      font-size: 17px;
      background: red;
      color: white;
    }`;
}