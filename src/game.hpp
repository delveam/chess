// TODO: we need one more layer of abstraction (um some sort of platform layer).
class Game {
  public:
    Game();
    void run();
    virtual bool should_close() = 0;
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void destroy() = 0;
    ~Game();
};
