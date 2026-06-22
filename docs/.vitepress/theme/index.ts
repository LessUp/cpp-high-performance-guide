import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import BaseAwareLink from './BaseAwareLink.vue'
import LanguageRedirect from './LanguageRedirect.vue'
import LanguageSwitcher from './LanguageSwitcher.vue'
import SectionIndex from './SectionIndex.vue'
import Citation from './Citation.vue'
import ComplexityBadge from './ComplexityBadge.vue'
import DiagramCanvas from './DiagramCanvas.vue'
import LandingHero from './LandingHero.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    // Register custom components globally
    app.component('BaseAwareLink', BaseAwareLink)
    app.component('SectionIndex', SectionIndex)
    app.component('Citation', Citation)
    app.component('ComplexityBadge', ComplexityBadge)
    app.component('DiagramCanvas', DiagramCanvas)
    app.component('LandingHero', LandingHero)
  },
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-top': () => h(LanguageRedirect),
      'nav-bar-content-after': () => h(LanguageSwitcher),
    })
  },
} satisfies Theme
