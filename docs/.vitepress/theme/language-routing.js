export const SUPPORTED_LANGS = [
  { code: 'en', label: 'English', path: '/en/' },
  { code: 'zh', label: '中文', path: '/zh/' },
]

const LANGUAGE_FALLBACKS = {
  zh: [
    { prefix: '/reference/api/', target: '/reference/' },
    { prefix: '/exercises/', target: '/' },
  ],
}

function ensureLeadingSlash(path) {
  return path.startsWith('/') ? path : `/${path}`
}

function normalizeFallbackMatchPath(path) {
  const normalizedPath = ensureLeadingSlash(path).replace(/\/+$/, '')
  return normalizedPath === '' ? '/' : `${normalizedPath}/`
}

export function normalizeSiteBase(siteBase = '/') {
  return siteBase.endsWith('/') ? siteBase : `${siteBase}/`
}

export function stripBase(path, siteBase = '/') {
  const normalizedPath = ensureLeadingSlash(path)
  const normalizedBase = normalizeSiteBase(siteBase)

  if (normalizedBase === '/') {
    return normalizedPath
  }

  const baseVariants = [...new Set([normalizedBase, normalizedBase.replace(/\/$/, '')])]

  for (const baseVariant of baseVariants) {
    if (!baseVariant) {
      continue
    }

    if (normalizedPath === baseVariant || normalizedPath === `${baseVariant}/`) {
      return '/'
    }

    if (normalizedPath.startsWith(`${baseVariant}/`)) {
      const stripped = normalizedPath.slice(baseVariant.length).replace(/^\/+/, '')
      return stripped ? `/${stripped}` : '/'
    }
  }

  return normalizedPath
}

export function stripLocale(path, supportedLangs = SUPPORTED_LANGS) {
  for (const { path: localePath } of supportedLangs) {
    if (path === localePath || path === localePath.slice(0, -1)) {
      return '/'
    }

    if (path.startsWith(localePath)) {
      const stripped = path.slice(localePath.length)
      return stripped ? `/${stripped.replace(/^\/+/, '')}` : '/'
    }
  }

  return path
}

export function withBase(path, siteBase = '/') {
  const normalizedBase = normalizeSiteBase(siteBase)

  if (normalizedBase === '/') {
    return ensureLeadingSlash(path)
  }

  return `${normalizedBase}${path.replace(/^\/+/, '')}`
}

export function resolveCurrentLanguage(routePath, siteBase = '/', supportedLangs = SUPPORTED_LANGS) {
  const currentPath = stripBase(routePath, siteBase)
  return supportedLangs.find((lang) => {
    const slashlessLocaleRoot = lang.path.replace(/\/$/, '')
    return currentPath === lang.path || currentPath === slashlessLocaleRoot || currentPath.startsWith(lang.path)
  }) ?? supportedLangs[0]
}

export function resolveLanguageTarget({
  routePath,
  siteBase = '/',
  targetLangPath,
  supportedLangs = SUPPORTED_LANGS,
}) {
  const currentPath = stripBase(routePath, siteBase)
  const pathWithoutLang = stripLocale(currentPath, supportedLangs)
  const targetLang = supportedLangs.find(lang => lang.path === targetLangPath)
  const normalizedFallbackPath = normalizeFallbackMatchPath(pathWithoutLang)
  const fallback = targetLang
    ? LANGUAGE_FALLBACKS[targetLang.code]?.find(rule =>
      normalizedFallbackPath.startsWith(normalizeFallbackMatchPath(rule.prefix)))
    : undefined
  const nextPath = fallback
    ? (fallback.target === '/'
      ? targetLangPath
      : `${targetLangPath}${fallback.target.replace(/^\/+/, '')}`)
    : (pathWithoutLang === '/'
      ? targetLangPath
      : `${targetLangPath}${pathWithoutLang.replace(/^\/+/, '')}`)

  return withBase(nextPath, siteBase)
}

export function createLanguageSwitcherLinks({
  routePath,
  siteBase = '/',
  supportedLangs = SUPPORTED_LANGS,
}) {
  const currentLang = resolveCurrentLanguage(routePath, siteBase, supportedLangs)

  return supportedLangs.map(lang => ({
    ...lang,
    targetPath: resolveLanguageTarget({
      routePath,
      siteBase,
      targetLangPath: lang.path,
      supportedLangs,
    }),
    isCurrent: lang.code === currentLang.code,
  }))
}
