import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { Sun, Moon, Sunrise, Sunset, Sparkles, Smile, RefreshCw } from 'lucide-react';

const WARM_MESSAGES = [
  "Wishing you a productive, inspiring, and joyful day ahead.",
  "Great things start with a single friendly hello.",
  "Ready to bring your ideas to life and create something wonderful.",
  "May your day be filled with clarity, creativity, and momentum."
];

export default function App() {
  const [name, setName] = useState('');
  const [quoteIndex, setQuoteIndex] = useState(0);
  const [timeInfo, setTimeInfo] = useState<{
    greeting: string;
    icon: typeof Sun;
    period: string;
    timeString: string;
  }>({
    greeting: 'Hello',
    icon: Sun,
    period: 'Day',
    timeString: ''
  });

  useEffect(() => {
    const updateTimeGreeting = () => {
      const now = new Date();
      const hour = now.getHours();
      const timeStr = now.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });

      if (hour >= 5 && hour < 12) {
        setTimeInfo({ greeting: 'Good morning', icon: Sunrise, period: 'Morning', timeString: timeStr });
      } else if (hour >= 12 && hour < 17) {
        setTimeInfo({ greeting: 'Good afternoon', icon: Sun, period: 'Afternoon', timeString: timeStr });
      } else if (hour >= 17 && hour < 21) {
        setTimeInfo({ greeting: 'Good evening', icon: Sunset, period: 'Evening', timeString: timeStr });
      } else {
        setTimeInfo({ greeting: 'Good night', icon: Moon, period: 'Night', timeString: timeStr });
      }
    };

    updateTimeGreeting();
    const timer = setInterval(updateTimeGreeting, 60000);
    return () => clearInterval(timer);
  }, []);

  const handleNextQuote = () => {
    setQuoteIndex((prev) => (prev + 1) % WARM_MESSAGES.length);
  };

  const IconComponent = timeInfo.icon;

  return (
    <div id="greeting-app" className="min-h-screen bg-stone-50 text-stone-900 flex flex-col justify-center items-center p-6 selection:bg-stone-200">
      <motion.main
        id="greeting-card"
        initial={{ opacity: 0, y: 16 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.5, ease: 'easeOut' }}
        className="w-full max-w-lg bg-white border border-stone-200 rounded-2xl p-8 sm:p-10 shadow-xs flex flex-col items-center text-center space-y-6"
      >
        {/* Badge & Period Icon */}
        <div id="time-badge" className="inline-flex items-center gap-2 px-3 py-1.5 rounded-full bg-stone-100 border border-stone-200/80 text-stone-700 text-xs font-medium">
          <IconComponent className="w-3.5 h-3.5 text-amber-600" />
          <span>{timeInfo.period}</span>
          {timeInfo.timeString && (
            <>
              <span className="text-stone-300">•</span>
              <span className="text-stone-500 font-mono text-xs">{timeInfo.timeString}</span>
            </>
          )}
        </div>

        {/* Main Display Heading */}
        <div className="space-y-2">
          <h1 id="greeting-heading" className="text-3xl sm:text-4xl font-semibold tracking-tight text-stone-900">
            {timeInfo.greeting}
            {name.trim() ? `, ${name.trim()}` : ''}!
          </h1>
          <p className="text-sm sm:text-base text-stone-500 max-w-sm mx-auto font-normal">
            Welcome! It's great to have you here today.
          </p>
        </div>

        {/* Interactive Quote / Well-wish */}
        <div id="quote-container" className="w-full p-4 rounded-xl bg-stone-50 border border-stone-200/60 relative">
          <AnimatePresence mode="wait">
            <motion.p
              key={quoteIndex}
              initial={{ opacity: 0, y: 4 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: -4 }}
              transition={{ duration: 0.2 }}
              className="text-stone-700 text-sm leading-relaxed"
            >
              "{WARM_MESSAGES[quoteIndex]}"
            </motion.p>
          </AnimatePresence>
          <button
            id="btn-refresh-quote"
            type="button"
            onClick={handleNextQuote}
            className="mt-3 inline-flex items-center gap-1.5 text-xs text-stone-500 hover:text-stone-900 transition-colors px-2.5 py-1 rounded-md hover:bg-stone-200/50"
            title="Show another message"
          >
            <RefreshCw className="w-3 h-3" />
            <span>Next inspiration</span>
          </button>
        </div>

        {/* Name personalization input */}
        <div className="w-full space-y-2 text-left">
          <label htmlFor="name-input" className="block text-xs font-medium text-stone-600">
            Personalize your greeting
          </label>
          <div className="relative">
            <input
              id="name-input"
              type="text"
              value={name}
              onChange={(e) => setName(e.target.value)}
              placeholder="Enter your name..."
              maxLength={30}
              className="w-full px-3.5 py-2.5 text-sm bg-white border border-stone-300 rounded-lg text-stone-900 placeholder:text-stone-400 focus:outline-none focus:ring-2 focus:ring-stone-900 focus:border-transparent transition-all"
            />
            {name && (
              <button
                id="btn-clear-name"
                type="button"
                onClick={() => setName('')}
                className="absolute right-2.5 top-1/2 -translate-y-1/2 text-xs text-stone-400 hover:text-stone-700 px-1.5 py-0.5 rounded"
              >
                Clear
              </button>
            )}
          </div>
        </div>

        {/* Bottom subtle prompt */}
        <div className="pt-2 border-t border-stone-100 w-full flex items-center justify-between text-xs text-stone-400">
          <span className="flex items-center gap-1">
            <Sparkles className="w-3.5 h-3.5 text-stone-400" />
            AI Studio Assistant
          </span>
          <span className="flex items-center gap-1">
            <Smile className="w-3.5 h-3.5 text-stone-400" />
            Ready to build
          </span>
        </div>
      </motion.main>
    </div>
  );
}

