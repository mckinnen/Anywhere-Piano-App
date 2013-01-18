class CGFxOpenGL
{
private:
  int m_windowWidth;
	int m_windowHeight;

public:
	CGFxOpenGL();
	virtual ~CGFxOpenGL();

	bool Init();
	bool Shutdown();

	void SetupProjection(int width, int height);

	void Prepare();
	void Render(float x, float y, int b);
	void Draw();
	void Picking(float x, float y, int b);
	
};
