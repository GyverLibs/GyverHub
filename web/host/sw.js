const CACHE_NAME = 'hub-cache-0.25b'

const CACHED_URLS = [
  
  '/',
  '/fa-solid-900.ttf',
  '/favicon.svg',
  '/index.html',
  '/script.js',
  '/style.css',

]

self.addEventListener('install', event => {
  event.waitUntil(async function () {
    const cache = await caches.open(CACHE_NAME);
    await cache.addAll(CACHED_URLS);
  }());
});

self.addEventListener('fetch', event => {
  const { request } = event;
  //if (!request.destination.length) return;
  if (!request.url.startsWith(self.location.href)) return;
  if (request.cache === 'only-if-cached' && request.mode !== 'same-origin') return;

  event.respondWith(async function () {
    const cache = await caches.open(CACHE_NAME);
    const cachedResponsePromise = await cache.match(request);
    const networkResponsePromise = fetch(request);
    if (request.url.startsWith(self.location.origin)) {
      event.waitUntil(async function () {
        const networkResponse = await networkResponsePromise;
        await cache.put(request, networkResponse.clone());
      }());
    }
    return cachedResponsePromise || networkResponsePromise;
  }());
});

self.addEventListener('activate', event => {
  event.waitUntil(async function () {
    const cacheNames = await caches.keys()
    await Promise.all(
      cacheNames.filter((cacheName) => {
        const deleteThisCache = cacheName !== CACHE_NAME;
        return deleteThisCache;
      }).map(cacheName => caches.delete(cacheName))
    )
  }());
});