#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SFML\Graphics.hpp>
#include <boost\variant.hpp>
#include <bitset>
#include <vector>

//#include "Entity.h"

using namespace std;
using namespace sf;

//class Entity;

namespace comp
{
	enum comps
	{
		NONE = -1,
		IS_ACTIVE,
		TRANSFORM,
		PHYSIC,
		GRAPHIC,
		ANIMATION,
		COLLIDER,
		NUMBER_OF_COMPONENTS
	};

	class component
	{
	public:
		virtual void onCreate() {}
	};

	class transform : public component
	{
	public:
		static const comps TYPE = TRANSFORM;
		transform(Vector2f _position = { 0, 0 }, float _heading = 0, Vector2f _origin = { 0, 0 })
			: position(_position), heading(_heading), origin(_origin) {}

		Vector2f position;
		float heading;
		Vector2f origin;

		bool hasChanged;

		void move(Vector2f transition) { position += transition; }
		void rotate(float rotation) { heading += rotation; }
	};

	class physic : public component
	{
	public:
		static const comps TYPE = PHYSIC;
		physic(Vector2f _force = { 0, 0 }, float _maxSpeed = 1, float _mass = 1)
			: force(_force), maxSpeed(_maxSpeed), mass(_mass) {}

		Vector2f force;
		float maxSpeed;
		float mass;

		void applyForce(Vector2f _force) { force += _force; }
		void reset() { force = { 0,0 }; }
	};

	class graphic : public component
	{
	public:
		static const comps TYPE = GRAPHIC;
		graphic(Texture* _texture = nullptr, IntRect _texLocation = { 0,0,1,1 }, Sprite _sprite = Sprite())
			: texture(_texture), texLocation(_texLocation), sprite(_sprite) {}

		Texture* texture;
		IntRect texLocation;
		Sprite sprite;

		void init() {
			sprite.setTexture(*texture);
			sprite.setOrigin(texLocation.width / 2, texLocation.height / 2); //set sprite origin based on texture rectangle
		}
	};

	class animation : public component
	{
	public:
		static const comps TYPE = ANIMATION;
		animation()
			: currentAnimation(0), currentFrame(0), state(IDLE), currentDuration(0) {}

		void update(float dt)
		{
			if (animationSets.size() == 0 || animationSets.at(currentAnimation).size() == 0)
				return;
			if (state != IDLE)
			{
				currentDuration += dt;

				if (currentDuration >= animationSets.at(currentAnimation).at(currentFrame))
				{
					if (state == ITERATE_ONCE && (currentFrame + 1 == animationSets.at(currentAnimation).size()))
					{
						currentAnimation = loopedAnimation;
						currentFrame = 0;
						currentDuration = 0;
					}
					else
					{
						currentDuration -= animationSets.at(currentAnimation).at(currentFrame);
						currentFrame = (currentFrame + 1) % animationSets.at(currentAnimation).size();
					}
				}
			}
		}

		void loop(int animationIndex)
		{
			if (animationIndex < 0 || animationIndex >= animationSets.size())
				return;
			loopedAnimation = animationIndex;
			currentFrame = 0;
			state = LOOPING;

		}

		void iterateOnce(int animationIndex)
		{
			if (animationIndex < 0 || animationIndex >= animationSets.size())
				return;
			currentAnimation = animationIndex;
			currentFrame = 0;
			state = ITERATE_ONCE;

		}

		void setIdle()
		{
			state = IDLE;
		}

		int addAnimations(int numberOfAnimation = 1)
		{
			for (int k = 0; k < numberOfAnimation; k++)
			{
				animationSets.push_back(frames());
			}

			return animationSets.size() - 1;
		}

		void addFrame(float duration, int animationIndex = -1)
		{
			if (animationIndex < 0 || animationIndex > animationSets.size())
			{
				animationSets.at(currentAnimation).push_back(duration);
			}
			else
			{
				animationSets.at(animationIndex).push_back(duration);
			}
		}

		int getFrame()
		{
			return currentFrame;
		}

		int getAnimation()
		{
			return currentAnimation;
		}

	private:
		int currentAnimation;
		int loopedAnimation;
		int currentFrame;
		float currentDuration;

		using frames = vector<float>;
		vector<frames> animationSets;
		/*
			animationSet	frames ->
			|
			v
		*/

		enum animationState
		{
			LOOPING,
			ITERATE_ONCE,
			IDLE
		};

		animationState state;
	};

	class collider : public component
	{
	public:
		static const comps TYPE = COLLIDER;
		FloatRect bounding;
	};

	using compVar = boost::variant<
		transform,
		physic,
		graphic,
		animation,
		collider
	>;

	static std::bitset<comps::NUMBER_OF_COMPONENTS> maskOf(comps type)
	{
		return std::bitset<comps::NUMBER_OF_COMPONENTS>(1 << type);
	}
}

#endif