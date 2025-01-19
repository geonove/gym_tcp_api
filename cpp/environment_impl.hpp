/**
 * @file environment_impl.hpp
 * @author Marcus Edel
 *
 * Implementation of miscellaneous environment routines.
 */
#ifndef GYM_ENVIRONMENT_IMPL_HPP
#define GYM_ENVIRONMENT_IMPL_HPP

// In case it hasn't been included yet.
#include "environment.hpp"
#include "messages.hpp"

namespace gym
{

  inline Environment::Environment() : renderValue(false)
  {
    // Nothing to do here.
  }

  inline Environment::Environment(const std::string &host, const std::string &port) : renderValue(false)
  {
    client.connect(host, port);
  }

  inline Environment::Environment(
      const std::string &host,
      const std::string &port,
      const std::string &environment) : renderValue(false)
  {
    client.connect(host, port);
    make(environment);
  }

  inline void Environment::make(const std::string &environment)
  {
    client.send(messages::EnvironmentName(environment));

    std::string json;
    client.receive(json);
    parser.parse(json);
    parser.environment(instance);

    observationSpace();
    actionSpace();

    observation_space.client(client);
    action_space.client(client);
    record_episode_stats.client(client);
  }

  inline void Environment::render()
  {
    if (renderValue)
    {
      renderValue = false;
    }
    else
    {
      renderValue = true;
    }
  }

  inline void Environment::close()
  {
    client.send(messages::EnvironmentClose());
  }

  inline const arma::mat &Environment::reset()
  {
    client.send(messages::EnvironmentReset());

    std::string json;
    client.receive(json);

    parser.parse(json);
    parser.observation(&observation_space, observation);

    return observation;
  }

  inline void Environment::step(const arma::mat &action)
  {
    std::cout << "ok 1" << std::endl;
    client.send(messages::Step(action, action_space, renderValue));
    std::cout << "ok 2" << std::endl;

    std::string json;
    try
    {
      client.receive(json);
    }
    catch (std::exception &e)
    {
      std::cout << "throwing>> " << e.what() << "\n" << std::endl;
      throw;
    }
    std::cout << "ok 3" << std::endl;
    parser.parse(json);
    std::cout << "ok 4" << std::endl;
    parser.observation(&observation_space, observation);
    std::cout << "ok 5" << std::endl;
    parser.info(reward, done, info);
    std::cout << "ok 6" << std::endl;
  }

  inline void Environment::seed(const size_t s)
  {
    client.send(messages::EnvironmentSeed(s));
  }

  inline void Environment::compression(const size_t compression)
  {
    client.compression(compression);
    client.send(messages::ServerCompression(compression));
  }

  inline void Environment::observationSpace()
  {
    client.send(messages::EnvironmentObservationSpace());

    std::string json;
    client.receive(json);

    parser.parse(json);
    parser.space(&observation_space);
  }

  inline void Environment::actionSpace()
  {
    client.send(messages::EnvironmentActionSpace());

    std::string json;
    client.receive(json);

    parser.parse(json);
    parser.space(&action_space);
  }

  inline std::string Environment::url()
  {
    client.send(messages::URL());

    std::string json;
    client.receive(json);

    std::string url;
    parser.parse(json);
    parser.url(url);

    return url;
  }

} // namespace gym

#endif
