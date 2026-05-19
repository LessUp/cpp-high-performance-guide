import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import BaseAwareLink from './BaseAwareLink.vue'
import LanguageRedirect from './LanguageRedirect.vue'
import LanguageSwitcher from './LanguageSwitcher.vue'
import SectionIndex from './SectionIndex.vue'
import Citation from './Citation.vue'
import CodeCompare from './CodeCompare.vue'
import ThemeAwareIcon from './ThemeAwareIcon.vue'
import AlgorithmCard from './AlgorithmCard.vue'
import ComplexityBadge from './ComplexityBadge.vue'
import DiagramCanvas from './DiagramCanvas.vue'
import ReferenceCard from './ReferenceCard.vue'
import LandingHero from './LandingHero.vue'
import Bibliography from './Bibliography.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    // Register custom components globally
    app.component('BaseAwareLink', BaseAwareLink)
    app.component('SectionIndex', SectionIndex)
    // New components for v2.0
    app.component('Citation', Citation)
    app.component('CodeCompare', CodeCompare)
    app.component('ThemeAwareIcon', ThemeAwareIcon)
    // Quantum Cobalt v3.0 components
    app.component('AlgorithmCard', AlgorithmCard)
    app.component('ComplexityBadge', ComplexityBadge)
    app.component('DiagramCanvas', DiagramCanvas)
    app.component('ReferenceCard', ReferenceCard)
    // Quantum Cobalt v4.0 components
    app.component('LandingHero', LandingHero)
    app.component('Bibliography', Bibliography)
  },
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-top': () => h(LanguageRedirect),
      'nav-bar-content-after': () => h(LanguageSwitcher),
    })
  },
} satisfies Theme
